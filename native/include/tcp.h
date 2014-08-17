#ifndef __TCP_H__
#define __TCP_H__

#include "base.h"
#include "handle.h"
#include "stream.h"
#include "net.h"
#include "callback.h"

namespace native
{
    namespace net
    {
        class tcp : public native::base::stream
        {
        public:
            template<typename X>
            tcp(X* x)
                : stream(x)
            { }

        public:
            tcp()
                : native::base::stream(new uv_tcp_t)
            {
                uv_tcp_init(uv_default_loop(), get<uv_tcp_t>());
            }

            tcp(native::loop& l)
                : native::base::stream(new uv_tcp_t)
            {
                uv_tcp_init(l.get(), get<uv_tcp_t>());
            }

            static std::shared_ptr<tcp> create()
            {
                return std::shared_ptr<tcp>(new tcp);
            }

            // TODO: bind and listen
            static std::shared_ptr<tcp> create_server(const std::string& ip, int port)
            {
                return nullptr;
            }

            bool nodelay(bool enable) { return uv_tcp_nodelay(get<uv_tcp_t>(), enable?1:0) == 0; }
            bool keepalive(bool enable, unsigned int delay) { return uv_tcp_keepalive(get<uv_tcp_t>(), enable?1:0, delay) == 0; }
            bool simultanious_accepts(bool enable) { return uv_tcp_simultaneous_accepts(get<uv_tcp_t>(), enable?1:0) == 0; }

            /** A general method which iAddr can be ip4 or ip6
             */
            bool bind(const sockaddr* iAddr, error& oError)
            {
                //TODO: add flags
                oError = uv_tcp_bind(get<uv_tcp_t>(), iAddr, 0);
                return !oError;
            }

            /** ip4 bind
             */
            bool bind(const std::string& ip, int port, error& oError)
            {
                ip4_addr addr;
                to_ip4_addr(ip.c_str(), port, addr, oError);
                if(oError) {
                    return false;
                }
                return bind(reinterpret_cast<const sockaddr*>(&addr), oError);
            }

            /** ip4 bind without error
             */
            bool bind(const std::string& ip, int port)
            {
                error err;
                return bind(ip, port, err);
            }

            /** ip6 bind with error
             */
            bool bind6(const std::string& ip, int port, error& oError)
            {
                ip6_addr addr;
                to_ip6_addr(ip.c_str(), port, addr, oError);
                if(oError) {
                    return false;
                }
                return bind(reinterpret_cast<const sockaddr*>(&addr), oError);
            }

            /** ip6 bind without error
             */
            bool bind6(const std::string& ip, int port)
            {
                error err;
                return bind(ip, port, err);
            }

            bool connect(const std::string& ip, int port, std::function<void(error)> callback, error& oError)
            {
                callbacks::store(get()->data, native::internal::uv_cid_connect, callback);
                ip4_addr addr;
                to_ip4_addr(ip, port, addr, oError);
                if (oError)
                {
                    return false;
                }
                uv_connect_t* connection_p = new uv_connect_t;
                oError = uv_tcp_connect(connection_p, get<uv_tcp_t>(), reinterpret_cast<const sockaddr*>(&addr), [](uv_connect_t* req, int status) {
                    callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_connect, error(status));
                    delete req;
                });
                if(oError)
                {
                    delete connection_p;
                }
                return !oError;
            }

            bool connect(const std::string& ip, int port, std::function<void(error)> callback)
            {
                error err;
                return connect(ip, port, callback, err);
            }

            bool connect6(const std::string& ip, int port, std::function<void(error)> callback, error& oError)
            {
                callbacks::store(get()->data, native::internal::uv_cid_connect6, callback);
                ip6_addr addr;
                to_ip6_addr(ip, port, addr, oError);
                if (oError)
                {
                    return false;
                }
                uv_connect_t* connection_p = new uv_connect_t;
                oError = uv_tcp_connect(connection_p, get<uv_tcp_t>(), reinterpret_cast<const sockaddr*>(&addr), [](uv_connect_t* req, int status) {
                    callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_connect6, error(status));
                    delete req;
                });
                if(oError)
                {
                    delete connection_p;
                }
                return !oError;
            }

            bool connect6(const std::string& ip, int port, std::function<void(error)> callback)
            {
                error err;
                return connect6(ip, port, callback, err);
            }

            bool getsockname(bool& ip4, std::string& ip, int& port)
            {
                struct sockaddr_storage addr;
                int len = sizeof(addr);
                if(uv_tcp_getsockname(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
                {
                    ip4 = (addr.ss_family == AF_INET);
                    if(ip4) return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
                    else return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
                }
                return false;
            }

            bool getpeername(bool& ip4, std::string& ip, int& port)
            {
                struct sockaddr_storage addr;
                int len = sizeof(addr);
                if(uv_tcp_getpeername(get<uv_tcp_t>(), reinterpret_cast<struct sockaddr*>(&addr), &len) == 0)
                {
                    ip4 = (addr.ss_family == AF_INET);
                    if(ip4) return from_ip4_addr(reinterpret_cast<ip4_addr*>(&addr), ip, port);
                    else return from_ip6_addr(reinterpret_cast<ip6_addr*>(&addr), ip, port);
                }
                return false;
            }
        };
    }
}

#endif
