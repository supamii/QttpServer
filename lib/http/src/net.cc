#include "native/net.h"
#include "native/error.h"

using namespace native;
using namespace net;

bool native::net::to_ip4_addr(const std::string& ip, int port, ip4_addr& oAddr, error& oError) {
  oError = uv_ip4_addr(ip.c_str(), port, &oAddr);
  return !oError;
}

bool native::net::to_ip6_addr(const std::string& ip, int port, ip6_addr& oAddr, error& oError) {
  oError = uv_ip6_addr(ip.c_str(), port, &oAddr);
  return !oError;
}

bool native::net::to_ip4_addr(const std::string& ip, int port, ip4_addr& oAddr) {
  error err = uv_ip4_addr(ip.c_str(), port, &oAddr);;
  return err;
}

bool native::net::to_ip6_addr(const std::string& ip, int port, ip6_addr& oAddr) {
  error err = uv_ip6_addr(ip.c_str(), port, &oAddr);
  return err;
}

bool native::net::from_ip4_addr(ip4_addr* src, std::string& ip, int& port) {
  char dest[16];
  if(uv_ip4_name(src, dest, 16) == 0) {
    ip = dest;
    port = static_cast<int>(ntohs(src->sin_port));
    return true;
  }
  return false;
}

bool native::net::from_ip6_addr(ip6_addr* src, std::string& ip, int& port) {
  char dest[46];
  if(uv_ip6_name(src, dest, 46) == 0) {
    ip = dest;
    port = static_cast<int>(ntohs(src->sin6_port));
    return true;
  }
  return false;
}
