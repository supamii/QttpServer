#include "native/stream.h"

using namespace native;
using namespace base;

bool stream::listen(std::function<void(native::error)> callback, int backlog)
{
    callbacks::store(get()->data, native::internal::uv_cid_listen, callback);
    return uv_listen(get<uv_stream_t>(), backlog, [](uv_stream_t* s, int status) {
        callbacks::invoke<decltype(callback)>(s->data, native::internal::uv_cid_listen, ((status != 0)?error(status):error()));
    }) == 0;
}

bool stream::accept(stream* client)
{
    return uv_accept(get<uv_stream_t>(), client->get<uv_stream_t>()) == 0;
}

bool stream::read_start(std::function<void(const char* buf, ssize_t len)> callback)
{
    return read_start<0>(callback);
}

bool stream::read_stop()
{
    return uv_read_stop(get<uv_stream_t>()) == 0;
}

// TODO: implement read2_start()

bool stream::write(const char* buf, int len, std::function<void(error)> callback)
{
    uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf), static_cast<size_t>(len) } };
    callbacks::store(get()->data, native::internal::uv_cid_write, callback);
    return uv_write(new uv_write_t, get<uv_stream_t>(), bufs, 1, [](uv_write_t* req, int status) {
        callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_write, ((status != 0)?error(status):error()));
        delete req;
    }) == 0;
}

bool stream::write(const std::string& buf, std::function<void(error)> callback)
{
    uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(buf.c_str()), buf.length()} };
    callbacks::store(get()->data, native::internal::uv_cid_write, callback);
    return uv_write(new uv_write_t, get<uv_stream_t>(), bufs, 1, [](uv_write_t* req, int status) {
        callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_write, ((status != 0)?error(status):error()));
        delete req;
    }) == 0;
}

bool stream::write(const std::vector<char>& buf, std::function<void(error)> callback)
{
    uv_buf_t bufs[] = { uv_buf_t { const_cast<char*>(&buf[0]), buf.size() } };
    callbacks::store(get()->data, native::internal::uv_cid_write, callback);
    return uv_write(new uv_write_t, get<uv_stream_t>(), bufs, 1, [](uv_write_t* req, int status) {
        callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_write, ((status !=0)?error(status):error()));
        delete req;
    }) == 0;
}

// TODO: implement write2()

bool stream::shutdown(std::function<void(error)> callback)
{
    callbacks::store(get()->data, native::internal::uv_cid_shutdown, callback);
    return uv_shutdown(new uv_shutdown_t, get<uv_stream_t>(), [](uv_shutdown_t* req, int status) {
        callbacks::invoke<decltype(callback)>(req->handle->data, native::internal::uv_cid_shutdown, ((status != 0)?error(status):error()));
        delete req;
    }) == 0;
}
