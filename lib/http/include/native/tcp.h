#ifndef __NATIVE_TCP_H__
#define __NATIVE_TCP_H__

#include "base.h"
#include "handle.h"
#include "loop.h"
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
            tcp();

            tcp(native::loop& l);

            static std::shared_ptr<tcp> create();

            // TODO: bind and listen
            // static std::shared_ptr<tcp> create_server(const std::string& ip, int port);

            bool nodelay(bool enable) { return uv_tcp_nodelay(get<uv_tcp_t>(), enable?1:0) == 0; }
            bool keepalive(bool enable, unsigned int delay) { return uv_tcp_keepalive(get<uv_tcp_t>(), enable?1:0, delay) == 0; }
            bool simultanious_accepts(bool enable) { return uv_tcp_simultaneous_accepts(get<uv_tcp_t>(), enable?1:0) == 0; }

            /** A general method which iAddr can be ip4 or ip6
             */
            bool bind(const sockaddr* iAddr, error& oError);

            /** ip4 bind
             */
            bool bind(const std::string& ip, int port, error& oError);
            bool bind(const std::string& ip, int port);

            /** ip6 bind with
             */
            bool bind6(const std::string& ip, int port, error& oError);
            bool bind6(const std::string& ip, int port);

            bool connect(const std::string& ip, int port, std::function<void(error)> callback, error& oError);
            bool connect(const std::string& ip, int port, std::function<void(error)> callback);

            bool connect6(const std::string& ip, int port, std::function<void(error)> callback, error& oError);
            bool connect6(const std::string& ip, int port, std::function<void(error)> callback);

            bool getsockname(bool& ip4, std::string& ip, int& port);
            bool getpeername(bool& ip4, std::string& ip, int& port);
        };
    }
}

#endif
