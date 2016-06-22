#ifndef QTTPHTTPREQUEST_H
#define QTTPHTTPREQUEST_H

#include "qttp_global.h"
#include "httpurl.h"

namespace qttp
{

class HttpServer;
class HttpData;

/**
 * TODO: FIXME: Should probably optionally allow users to omit this entirely
 * since it may add more bloat if they don't want or need it!
 *
 */
class QTTPSHARED_EXPORT HttpRequest
{
    friend class HttpServer;
    friend class HttpData;

  private:
    HttpRequest(native::http::request* req);

  public:
    ~HttpRequest();

    const native::http::url_obj& url() const;
    const std::string& get_header(const std::string& key) const;
    bool get_header(const std::string& key, std::string& value) const;
    std::string get_body() const;
    std::stringstream& get_raw_body();
    const std::string& get_method() const;

    uint64_t get_timestamp() const;

    const HttpUrl& getUrl() const;
    const QString& getBody() const;
    const QString& getMethod() const;

  private:
    Assert<native::http::request> m_Assertion;
    native::http::request* m_Request;
    mutable QSharedPointer<HttpUrl> m_HttpUrl;
    mutable QSharedPointer<QString> m_Method;
    mutable QSharedPointer<QString> m_Body;
};

}
#endif // QTTPHTTPREQUEST_H
