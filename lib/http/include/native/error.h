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
    {
    }

    virtual ~exception() {
    }

    const std::string& message() const {
      return message_;
    }

  private:
    std::string message_;
};

class error
{
  public:
    error() : uv_err_(0) {
    }
    error(int iErrCode) : uv_err_(iErrCode) {
    }
    ~error() = default;
    bool isError() {
      return (uv_err_ != 0);
    }
    operator bool() {
      return isError();
    }
    virtual error& operator=(int iErrCode)
    {
      setError(iErrCode);
      return *this;
    }

    void setError(int iErrCode) {
      uv_err_ = iErrCode;
    }
    int code() const {
      return uv_err_;
    }

    const char* name() const {
      return uv_err_ < 0 ? uv_err_name(uv_err_) : "Unknown error name";
    }
    const char* str() const {
      return uv_err_ < 0 ? uv_strerror(uv_err_) : "Unknown error";
    }

  private:
    int uv_err_;
};
}


#endif
