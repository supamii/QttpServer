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
    HttpUrl(native::http::QttpRequest* req);

  public:
    ~HttpUrl();

    /**
     * @brief This defeats the purpose of lazily initializing members.
     */
    void initialize();

    const QByteArray& getSchema() const;
    const QByteArray& getHost() const;
    qint32 getPort() const;
    const QByteArray& getPath() const;
    const QByteArray& getQuery() const;
    const QByteArray& getFragment() const;

  private:
    QTTP_DECLARE_ASSERT_MEMBER(native::http::QttpRequest)

    const native::http::QttpRequest * m_Request;

    /// Mutable because this is inteded to help cache and reduce QString conversions.
    mutable QByteArray m_Schema;

    /// Mutable because this is inteded to help cache and reduce QString conversions.
    mutable QByteArray m_Host;

    /// Mutable because this is inteded to help cache and reduce QString conversions.
    mutable QByteArray m_Path;

    /// Mutable because this is inteded to help cache and reduce QString conversions.
    mutable QByteArray m_Query;

    /// Mutable because this is inteded to help cache and reduce QString conversions.
    mutable QByteArray m_Fragment;
};

}

#endif // QTTPHTTPURL_H
