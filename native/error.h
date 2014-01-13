#ifndef __ERROR_H__
#define __ERROR_H__

#include "base.h"

namespace native
{
    class exception
    {
    public:
        exception(const std::string& message)
            : message_(message)
        {}

        virtual ~exception() {}

        const std::string& message() const { return message_; }

    private:
        std::string message_;
    };

    class error
    {
    public:
        error() : uv_err_(), is_error(false) {}
        error(uv_errno_t e) : uv_err_(e), is_error(true) {}
        error(int iErrCode) : uv_err_(), is_error(iErrCode!=0) {
            if(is_error) {
                uv_err_ = static_cast<uv_errno_t>(iErrCode);
            }
        }
        ~error() = default;
        bool isError() { return is_error; }
        operator bool() { return isError(); }
        virtual error& operator=(int iErrCode) {
            setError(iErrCode);
            return *this;
        }

        void setError(int iErrCode) {
            if(iErrCode == 0){
                is_error = false;
                return;
            }
            is_error = true;
            uv_err_ = static_cast<uv_errno_t>(iErrCode);
        }
        int code() const { return uv_err_; }

        const char* name() const { return is_error ? uv_err_name(uv_err_) : 0; }
        const char* str() const { return is_error ? uv_strerror(uv_err_) : 0; }

    private:
        uv_errno_t uv_err_;
        //FIXME: remove this bool field. this is temporary till wil find the final solution
        bool is_error;
    };
}


#endif
