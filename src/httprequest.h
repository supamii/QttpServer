#ifndef QTTPHTTPREQUEST_H
#define QTTPHTTPREQUEST_H

#include "qttp_global.h"
#include "httpurl.h"

namespace qttp
{

class HttpServer;
class HttpData;

class QTTPSHARED_EXPORT HttpRequest
{
    friend class HttpServer;
    friend class HttpData;

  private:
    HttpRequest(native::http::request* req);
    ~HttpRequest();

  public:
    const HttpUrl & url() const;

    const std::string& get_header(const std::string& key) const;
    bool get_header(const std::string& key, std::string& value) const;

    std::string get_body (void) const;
    std::stringstream& get_raw_body (void);

    const std::string& get_method (void) const;

    uint64_t get_timestamp(void) const;

  private:
    Assert<native::http::request> m_Assertion;
    native::http::request* m_Request;
    HttpUrl m_HttpUrl;
};

}
#endif // QTTPHTTPREQUEST_H
