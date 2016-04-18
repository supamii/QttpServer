#ifndef QTTPHTTPURL_H
#define QTTPHTTPURL_H

#include "qttp_global.h"

namespace qttp
{

class HttpData;
class HttpRequest;
class HttpResponse;

/**
 * @brief The HttpUrl class
 *
 * TODO: Still need to figure out if we can optimize mem usage.
 */
class QTTPSHARED_EXPORT HttpUrl
{
    friend class HttpData;
    friend class HttpRequest;
    friend class HttpResponse;

  private:
    HttpUrl(native::http::request* req);
    ~HttpUrl();

  public:
    QString& schema() const;
    QString& host() const;
    qint32 port() const;
    QString& path() const;
    QString& query() const;
    QString& fragment() const;

  private:
    Assert<native::http::request> m_Assertion;
    const native::http::request* m_Request;
    QString m_Schema;
    QString m_Host;
    QString m_Path;
    QString m_Query;
    QString m_Fragment;
};

}

#endif // QTTPHTTPURL_H
