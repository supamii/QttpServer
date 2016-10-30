#include "native/tcp.h"

#ifdef SSL_TLS_UV
  #include <uv_tls.h>
  #include <iostream>
#endif

using namespace native;
using namespace net;

tcp::tcp() :
  native::base::stream(new uv_tcp_t)
{
  uv_tcp_init(uv_default_loop(), get<uv_tcp_t>());
}

tcp::~tcp()
{
}

tcp::tcp(native::loop& l) :
  native::base::stream(new uv_tcp_t)
{
  uv_tcp_init(l.get(), get<uv_tcp_t>());
}

std::shared_ptr<tcp> tcp::create()
{
  return std::shared_ptr<tcp>(new tcp);
}

// TODO: bind and listen
//std::shared_ptr<tcp> tcp::create_server(const std::string& ip, int port)
//{
//    return nullptr;
//}

bool tcp::bind(const sockaddr* iAddr, error& oError)
{
  uv_tcp_t* listener = get<uv_tcp_t>();

#ifdef SSL_TLS_UV
  evt_ctx_t* ctx = new evt_ctx_t;
  // TODO: FIXME: This only needs to occur once.
  int result = evt_ctx_init_ex(ctx, "server-cert.pem", "server-key.pem");
  if(result != 1) {
    std::cout << "Warning! TLS certificates did not load [" << result << "]" << std::endl;
    delete (evt_ctx_t*) ctx;
  } else {
    evt_ctx_set_nio(ctx, NULL, ::uv_tls_writer);
    if(listener->data != nullptr) {
      std::cout << "Warning! Potential memory leak";
    }
    // TODO: FIXME: Callbacks is set here, need to find another place to store it.
    listener->data = ctx;
  }
#endif

  // TODO: add flags
  oError = uv_tcp_bind(listener, iAddr, 0);
  if(oError)
  {
    PRINT_NN_ERROR(oError);
    return false;
  }
  return true;
}

bool tcp::bind(const std::string& ip, int port, error& oError)
{
  ip4_addr addr;
  to_ip4_addr(ip.c_str(), port, addr, oError);
  if(oError) {
    PRINT_NN_ERROR(oError);
    return false;
  }
  return bind(reinterpret_cast<const sockaddr*>(&addr), oError);
}

bool tcp::bind(const std::string& ip, int port)
{
  error err;
  return bind(ip, port, err);
}

bool tcp::bind6(const std::string& ip, int port, error& oError)
{
  ip6_addr addr;
  to_ip6_addr(ip.c_str(), port, addr, oError);
  if(oError) {
    PRINT_NN_ERROR(oError);
    return false;
  }
  return bind(reinterpret_cast<const sockaddr*>(&addr), oError);
}

bool tcp::bind6(const std::string& ip, int port)
{
  error err;
  return bind(ip, port, err);
}

bool tcp::connect(const std::string& ip, int port, std::function<void(error)> callback, error& oError)
{
  callbacks::store(get()->data, native::internal::uv_cid_connect, callback);

  ip4_addr addr;
  to_ip4_addr(ip, port, addr, oError);
  if (oError)
  {
    PRINT_NN_ERROR(oError);
    return false;
  }

  uv_connect_t* connection_p = new uv_connect_t;
  oError = uv_tcp_connect(connection_p, get<uv_tcp_t>(), reinterpret_cast<const sockaddr*>(&addr), [](uv_connect_t* req, int status) {
    callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_connect, error(status));
    delete req;
  });
  if(oError)
  {
    PRINT_NN_ERROR(oError);
    delete connection_p;
  }
  return !oError;
}

bool tcp::connect(const std::string& ip, int port, std::function<void(error)> callback)
{
  error err;
  return connect(ip, port, callback, err);
}

bool tcp::connect6(const std::string& ip, int port, std::function<void(error)> callback, error& oError)
{
  callbacks::store(get()->data, native::internal::uv_cid_connect6, callback);

  ip6_addr addr;
  to_ip6_addr(ip, port, addr, oError);
  if (oError)
  {
    PRINT_NN_ERROR(oError);
    return false;
  }

  uv_connect_t* connection_p = new uv_connect_t;
  oError = uv_tcp_connect(connection_p, get<uv_tcp_t>(), reinterpret_cast<const sockaddr*>(&addr), [](uv_connect_t* req, int status) {
    callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_connect6, error(status));
    delete req;
  });
  if(oError)
  {
    PRINT_NN_ERROR(oError);
    delete connection_p;
  }
  return !oError;
}

bool tcp::connect6(const std::string& ip, int port, std::function<void(error)> callback)
{
  error err;
  return connect6(ip, port, callback, err);
}

bool tcp::getsockname(bool& ip4, std::string& ip, int& port) const
{
  struct sockaddr_storage addr;
  int len = sizeof(addr);
  if(uv_tcp_getsockname(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
  {
    ip4 = (addr.ss_family == AF_INET);
    return (ip4) ?
           from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port) :
           from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
  }
  return false;
}

bool tcp::getpeername(bool& ip4, std::string& ip, int& port) const
{
  struct sockaddr_storage addr;
  int len = sizeof(addr);
  if(uv_tcp_getpeername(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
  {
    ip4 = (addr.ss_family == AF_INET);
    return (ip4) ?
           from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port) :
           from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
  }
  return false;
}
