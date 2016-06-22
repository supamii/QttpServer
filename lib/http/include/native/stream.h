#ifndef __NATIVE_STREAM_H__
#define __NATIVE_STREAM_H__

#include "base.h"
#include "error.h"
#include "handle.h"
#include "callback.h"

#include <algorithm>

namespace native
{
    namespace base
    {
        class stream : public handle
        {
        public:
            template<typename T>
            stream(T* x)
                : handle(x)
            { }

            bool listen(std::function<void(native::error)> callback, int backlog=128);

            bool accept(stream* client);

            bool read_start(std::function<void(const char* buf, ssize_t len)> callback);
            template<size_t max_alloc_size> bool read_start(std::function<void(const char* buf, ssize_t len)> callback);

            bool read_stop();

            // TODO: implement read2_start()

            bool write(const char* buf, int len, std::function<void(error)> callback);
            bool write(const std::string& buf, std::function<void(error)> callback);
            bool write(const std::vector<char>& buf, std::function<void(error)> callback);

            // TODO: implement write2()

            bool shutdown(std::function<void(error)> callback);
        };

        // template body

        template<size_t max_alloc_size>
        bool stream::read_start(std::function<void(const char* buf, ssize_t len)> callback)
        {
            callbacks::store(get()->data, native::internal::uv_cid_read_start, callback);

            return uv_read_start(get<uv_stream_t>(),
                [](uv_handle_t*, size_t suggested_size, uv_buf_t* buf){
                    auto size = (std::max)(suggested_size, max_alloc_size);
                    buf->base = new char[size];
                    buf->len = size;
                    if((buf->base == NULL) && (buf->len > 0))
                    {
                        assert(0);
                        //TODO: error
                    }
                },
                [](uv_stream_t* s, ssize_t nread, const uv_buf_t* buf){
                    if(nread < 0)
                    {
                        //TODO: to find workaround
                        //assert(uv_last_error(s->loop).code == UV_EOF);
                        callbacks::invoke<decltype(callback)>(s->data, native::internal::uv_cid_read_start, nullptr, nread);
                    }
                    else if(nread >= 0)
                    {
                        callbacks::invoke<decltype(callback)>(s->data, native::internal::uv_cid_read_start, buf->base, nread);
                    }
                    delete buf->base;
                }) == 0;
        }
    }
}

#endif
