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
 */
class QTTPSHARED_EXPORT HttpUrl
{
  friend class HttpData;
  friend class HttpRequest;
  friend class HttpResponse;

  private:
    HttpUrl(native::http::QttpRequest* req);

  public:
    ~HttpUrl();

    const QByteArray& getSchema() const;
    const QByteArray& getHost() const;
    qint32 getPort() const;
    const QByteArray& getPath() const;
    const QByteArray& getQuery() const;
    const QByteArray& getFragment() const;

  private:
    QTTP_DECLARE_ASSERT_MEMBER(native::http::QttpRequest)

    const native::http::QttpRequest * m_Request;
    QByteArray m_Schema;
    QByteArray m_Host;
    QByteArray m_Path;
    QByteArray m_Query;
    QByteArray m_Fragment;
};

}

#endif // QTTPHTTPURL_H
