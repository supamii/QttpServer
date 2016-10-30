#ifndef QTTPHTTPDATA_H
#define QTTPHTTPDATA_H

#include "qttp_global.h"
#include "httprequest.h"
#include "httpresponse.h"

namespace qttp
{

// Forward declaration
class HttpServer;

/**
 *
 * @brief This is the main access point for building a the response.  This is
 * also an avenue for getting information about the request.
 *
 * @todo Add a friend stream operator for pretty-printing
 */
class QTTPSHARED_EXPORT HttpData
{
  friend class HttpServer;

  private:
    /**
     * @brief Private constructor because the underlying native::http::request
     * and native::http::response objects are privately managed as well.
     *
     * Throws an exception if arguments are NULL.
     */
    HttpData(native::http::QttpRequest*, native::http::QttpResponse*);
    ~HttpData();

  public:

    const HttpRequest& getRequest() const;
    HttpRequest& getRequest();

    const HttpResponse& getResponse() const;
    HttpResponse& getResponse();

    /**
     * @brief Quickly and easily assign JSON responses.
     *
     * This is the same as getResponse().setJson();
     */
    void setResponse(const QJsonObject& json);

    //! Quick and easy way to set error messages.
    void setErrorResponse(const QString& msg);
    void setErrorResponse(const QString& msg, HttpError code);
    void setErrorResponse(const QJsonObject& json);
    void setErrorResponse(const QJsonObject& json, HttpError code);

    const QUuid& getUid() const;
    void setTimestamp(const QDateTime& timestamp);
    const QDateTime& getTimestamp() const;
    const QTime& getTime() const;

  private:

    HttpRequest m_HttpRequest;
    HttpResponse m_HttpResponse;
    QUuid m_Uid;
    QDateTime m_Timestamp;
    QTime m_Time;
};

}
#endif // QTTPHTTPDATA_H
