#ifndef __FS_H__
#define __FS_H__

#include "base.h"
#include <functional>
#include <algorithm>
#include <fcntl.h>
#include "callback.h"

#include "native/error.h"

namespace native
{
    // TODO: implement functions that accept loop pointer as extra argument.
    namespace fs
    {
        typedef uv_file file_handle;

        static const int read_only = O_RDONLY;
        static const int write_only = O_WRONLY;
        static const int read_write = O_RDWR;
        static const int append = O_APPEND;
        static const int create = O_CREAT;
        static const int excl = O_EXCL;
        static const int truncate = O_TRUNC;
#ifdef O_NOFOLLOW
        static const int no_follow = O_NOFOLLOW;
#endif
#ifdef O_DIRECTORY
        static const int directory = O_DIRECTORY;
#endif
#ifdef O_NOATIME
        static const int no_access_time = O_NOATIME;
#endif
#ifdef O_LARGEFILE
        static const int large_large = O_LARGEFILE;
#endif

        namespace internal
        {
            template<typename callback_t>
            uv_fs_t* create_req(callback_t callback, void* data=nullptr)
            {
                auto req = new uv_fs_t;
                req->data = new callbacks(1);
                assert(req->data);
                callbacks::store(req->data, 0, callback, data);

                return req;
            }

            template<typename callback_t, typename ...A>
            typename std::result_of<callback_t(A...)>::type invoke_from_req(uv_fs_t* req, A&& ... args)
            {
                return callbacks::invoke<callback_t>(req->data, 0, std::forward<A>(args)...);
            }

            template<typename callback_t, typename data_t>
            data_t* get_data_from_req(uv_fs_t* req)
            {
                return reinterpret_cast<data_t*>(callbacks::get_data<callback_t>(req->data, 0));
            }

            template<typename callback_t, typename data_t>
            void delete_req(uv_fs_t* req)
            {
                delete reinterpret_cast<data_t*>(callbacks::get_data<callback_t>(req->data, 0));
                delete reinterpret_cast<callbacks*>(req->data);
                uv_fs_req_cleanup(req);
                delete req;
            }

            template<typename callback_t, typename data_t>
            void delete_req_arr_data(uv_fs_t* req)
            {
                delete[] reinterpret_cast<data_t*>(callbacks::get_data<callback_t>(req->data, 0));
                delete reinterpret_cast<callbacks*>(req->data);
                uv_fs_req_cleanup(req);
                delete req;
            }

            void delete_req(uv_fs_t* req)
            {
                delete reinterpret_cast<callbacks*>(req->data);
                uv_fs_req_cleanup(req);
                delete req;
            }

            struct rte_context
            {
                fs::file_handle file;
                const static int buflen = 32;
                char buf[buflen];
                std::string result;
            };

            template<typename callback_t>
            void rte_cb(uv_fs_t* req)
            {
                assert(req->fs_type == UV_FS_READ);

                auto ctx = reinterpret_cast<rte_context*>(callbacks::get_data<callback_t>(req->data, 0));
                if(req->result < 0)
                {
                    // system error
                    invoke_from_req<callback_t>(req, std::string(), error(req->result));
                    delete_req<callback_t, rte_context>(req);
                }
                else if(req->result == 0)
                {
                    // EOF
                    invoke_from_req<callback_t>(req, ctx->result, error());
                    delete_req<callback_t, rte_context>(req);
                }
                else
                {
                    ctx->result.append(std::string(ctx->buf, req->result));

                    uv_fs_req_cleanup(req);
                    const uv_buf_t iov = uv_buf_init(ctx->buf, rte_context::buflen);

                    error err(uv_fs_read(uv_default_loop(), req, ctx->file, &iov, 1, ctx->result.length(), rte_cb<callback_t>));
                    if(err)
                    {
                        invoke_from_req<callback_t>(req, std::string(), err);
                        delete_req<callback_t, rte_context>(req);
                    }
                }
            }
        }

        bool open(const std::string& path, int flags, int mode, std::function<void(native::fs::file_handle fd, error e)> callback);

        bool read(file_handle fd, size_t len, off_t offset, std::function<void(const std::string& str, error e)> callback);

        bool write(file_handle fd, const char* buf, size_t len, off_t offset, std::function<void(int nwritten, error e)> callback);

        bool read_to_end(file_handle fd, std::function<void(const std::string& str, error e)> callback);

        bool close(file_handle fd, std::function<void(error e)> callback);

        bool unlink(const std::string& path, std::function<void(error e)> callback);

        bool mkdir(const std::string& path, int mode, std::function<void(error e)> callback);

        bool rmdir(const std::string& path, std::function<void(error e)> callback);

        bool rename(const std::string& path, const std::string& new_path, std::function<void(error e)> callback);

        bool chmod(const std::string& path, int mode, std::function<void(error e)> callback);

        bool chown(const std::string& path, int uid, int gid, std::function<void(error e)> callback);

#if 0
        bool readdir(const std::string& path, int flags, std::function<void(error e)> callback)
        {
            auto req = internal::create_req(callback);
            if(uv_fs_readdir(uv_default_loop(), req, path.c_str(), flags, [](uv_fs_t* req){
                assert(req->fs_type == UV_FS_READDIR);
                internal::invoke_from_req<decltype(callback)>(req, req->result<0?error(req->result):error());
                internal::delete_req(req);
            })) {
                internal::delete_req(req);
                return false;
            }
            return true;
        }

        bool stat(const std::string& path, std::function<void(error e)> callback)
        {
            auto req = internal::create_req(callback);
            if(uv_fs_stat(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
                assert(req->fs_type == UV_FS_STAT);
                internal::invoke_from_req<decltype(callback)>(req, req->result<0?error(req->result):error());
                internal::delete_req(req);
            })) {
                internal::delete_req(req);
                return false;
            }
            return true;
        }

        bool fstat(const std::string& path, std::function<void(error e)> callback)
        {
            auto req = internal::create_req(callback);
            if(uv_fs_fstat(uv_default_loop(), req, path.c_str(), [](uv_fs_t* req){
                assert(req->fs_type == UV_FS_FSTAT);
                internal::invoke_from_req<decltype(callback)>(req, req->result<0?error(req->result):error());
                internal::delete_req(req);
            })) {
                internal::delete_req(req);
                return false;
            }
            return true;
        }
#endif
    }

    class file
    {
    public:
        static bool read(const std::string& path, std::function<void(const std::string& str, error e)> callback)
        {
            return fs::open(path.c_str(), fs::read_only, 0, [=](fs::file_handle fd, error e) {
                if(e)
                {
                    callback(std::string(), e);
                }
                else
                {
                    if(!fs::read_to_end(fd, callback))
                    {
                        // failed to initiate read_to_end()
                        //TODO: this should not happen for async (callback provided). Temporary return unknown error. To resolve this.
                        //callback(std::string(), error(uv_last_error(uv_default_loop())));
                        callback(std::string(), error(UV__UNKNOWN));
                    }
                }
            });
        }

        static bool write(const std::string& path, const std::string& str, std::function<void(int nwritten, error e)> callback)
        {
            return fs::open(path.c_str(), fs::write_only|fs::create, 0664, [=](fs::file_handle fd, error e) {
                if(e)
                {
                    callback(0, e);
                }
                else
                {
                    if(!fs::write(fd, str.c_str(), str.length(), 0, callback))
                    {
                        // failed to initiate read_to_end()
                        //TODO: this should not happen for async (callback provided). Temporary return unknown error. To resolve this.
                        //callback(0, error(uv_last_error(uv_default_loop())));
                        callback(0, error(UV__UNKNOWN));
                    }
                }
            });
        }
    };
}

#endif
