#include "native/fs.h"

using namespace native;
using namespace native::fs;

bool native::fs::open(const std::string& path, int flags, int mode, std::function<void(native::fs::file_handle fd, native::error e)> callback)
{
    auto req = internal::create_req(callback);
    native::error err;
    if((err = uv_fs_open(uv_default_loop(), req, path.c_str(), flags, mode, [](uv_fs_t* req) {
        assert(req->fs_type == UV_FS_OPEN);

        if(req->result < 0) internal::invoke_from_req<decltype(callback)>(req, file_handle(-1), native::error(req->result));
        else internal::invoke_from_req<decltype(callback)>(req, req->result, native::error());

        internal::delete_req(req);
    }))) {
        // failed to initiate uv_fs_open()
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::read(file_handle fd, size_t len, off_t offset, std::function<void(const std::string& str, native::error e)> callback)
{
    auto buf = new char[len];
    auto req = internal::create_req(callback, buf);
    const uv_buf_t iov = uv_buf_init(buf, len);
    if(uv_fs_read(uv_default_loop(), req, fd, &iov, 1, offset, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_READ);

        if(req->result < 0)
        {
            // system error
            internal::invoke_from_req<decltype(callback)>(req, std::string(), native::error(req->result));
        }
        else if(req->result == 0)
        {
            // EOF
            internal::invoke_from_req<decltype(callback)>(req, std::string(), native::error(UV_EOF));
        }
        else
        {
            auto buf = internal::get_data_from_req<decltype(callback), char>(req);
            internal::invoke_from_req<decltype(callback)>(req, std::string(buf, req->result), native::error());
        }

        internal::delete_req_arr_data<decltype(callback), char>(req);
    })) {
        // failed to initiate uv_fs_read()
        internal::delete_req_arr_data<decltype(callback), char>(req);
        return false;
    }
    return true;
}

bool native::fs::write(file_handle fd, const char* buf, size_t len, off_t offset, std::function<void(int nwritten, native::error e)> callback)
{
    auto req = internal::create_req(callback);
    const uv_buf_t iov = uv_buf_init(const_cast<char*>(buf), len);

    // TODO: const_cast<> !!
    if(uv_fs_write(uv_default_loop(), req, fd, &iov, 1, offset, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_WRITE);

        if(req->result)
        {
            internal::invoke_from_req<decltype(callback)>(req, 0, native::error(req->result));
        }
        else
        {
            internal::invoke_from_req<decltype(callback)>(req, req->result, native::error());
        }

        internal::delete_req(req);
    })) {
        // failed to initiate uv_fs_write()
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::read_to_end(file_handle fd, std::function<void(const std::string& str, native::error e)> callback)
{
    auto ctx = new internal::rte_context;
    ctx->file = fd;
    auto req = internal::create_req(callback, ctx);
    const uv_buf_t iov = uv_buf_init(ctx->buf, internal::rte_context::buflen);

    if(uv_fs_read(uv_default_loop(), req, fd, &iov, 1, 0, internal::rte_cb<decltype(callback)>)) {
        // failed to initiate uv_fs_read()
        internal::delete_req<decltype(callback), internal::rte_context>(req);
        return false;
    }
    return true;
}

bool native::fs::close(file_handle fd, std::function<void(native::error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_close(uv_default_loop(), req, fd, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_CLOSE);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0? native::error(req->result): native::error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::unlink(const std::string& path, std::function<void(native::error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_unlink(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_UNLINK);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0? native::error(req->result): native::error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::mkdir(const std::string& path, int mode, std::function<void(native::error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_mkdir(uv_default_loop(), req, path.c_str(), mode, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_MKDIR);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0? native::error(req->result): native::error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::rmdir(const std::string& path, std::function<void(native::error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_rmdir(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_RMDIR);
        internal::invoke_from_req<decltype(callback)>(req, req->result < 0? native::error(req->result): native::error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::rename(const std::string& path, const std::string& new_path, std::function<void(native::error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_rename(uv_default_loop(), req, path.c_str(), new_path.c_str(), [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_RENAME);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0? native::error(req->result): native::error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::chmod(const std::string& path, int mode, std::function<void(native::error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_chmod(uv_default_loop(), req, path.c_str(), mode, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_CHMOD);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0? native::error(req->result): native::error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}

bool native::fs::chown(const std::string& path, int uid, int gid, std::function<void(native::error e)> callback)
{
    auto req = internal::create_req(callback);
    if(uv_fs_chown(uv_default_loop(), req, path.c_str(), uid, gid, [](uv_fs_t* req){
        assert(req->fs_type == UV_FS_CHOWN);
        internal::invoke_from_req<decltype(callback)>(req, req->result<0? native::error(req->result): native::error());
        internal::delete_req(req);
    })) {
        internal::delete_req(req);
        return false;
    }
    return true;
}
