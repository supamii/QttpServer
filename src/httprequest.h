#ifndef QTTPHTTPREQUEST_H
#define QTTPHTTPREQUEST_H

#include "qttp_global.h"
#include "httpurl.h"

namespace qttp
{

// Forward declaration
class HttpServer;
class HttpData;

/**
 * Holds all related information relating to the request (in-bound).
 */
class QTTPSHARED_EXPORT HttpRequest
{
  friend class HttpServer;
  friend class HttpData;

QTTP_PRIVATE:

  HttpRequest(native::http::QttpRequest* req);

  public:

    ~HttpRequest();

    bool containsHeader(const QString& key) const;
    const QString& getHeader(const QString& key) const;
    bool getHeader(const QString& key, QString& value) const;
    uint64_t getTimestamp() const;
    const HttpUrl& getUrl() const;
    const QString &getMethodStr() const;

    /**
     * NOT THREAD SAFE - this value is lazily initialized so user beware.  This
     * should be called before dispatching to multiple threads (if there's some
     * reason for multiple threads).
     *
     * This will return the HTTP method based on char-checking instead of a
     * full string-comparison.
     */
    HttpMethod getMethod(bool strictComparison = false) const;

    /**
     * @brief Highly recommended!  This builds and returns a QJsonObject that
     * consolidates query-string parameters and the reqeust body (JSON only).
     * This avoids having to grab from the query string and parsing through the
     * request body separately.
     *
     * NOT THREAD SAFE - this value is lazily initialized so user beware.  This
     * should be called before dispatching to multiple threads (if there's some
     * reason for multiple threads).
     */
    const QJsonDocument& getJson() const;

    const QByteArray& getBody() const;

    QUrlQuery& getQuery();
    const QUrlQuery& getQuery() const;

QTTP_PROTECTED:

    /**
     * @brief This will swap data - beware
     */
    void setQuery(QUrlQuery&);

  public:

    /**
     * @brief Beware that accessing this after invoking finishResponse() will
     * result in an exception since alloated memory will be NULL.  Also be sure
     * NOT to save/use this reference outside of the lifetime of HttpData.
     */
    native::http::QttpRequest* getRequest();

QTTP_PRIVATE:

    QTTP_DECLARE_ASSERT_MEMBER(native::http::QttpRequest)

    native::http::QttpRequest * m_Request;
    HttpUrl m_HttpUrl;
    mutable HttpMethod m_MethodEnum;
    mutable QJsonDocument m_Json;
    QUrlQuery m_Query;
};

}
#endif // QTTPHTTPREQUEST_H
