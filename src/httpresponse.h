#ifndef QTTPHTTPRESPONSE_H
#define QTTPHTTPRESPONSE_H

#include "qttp_global.h"

namespace qttp
{

// Forward declaration
class HttpServer;
class HttpData;

/**
 * Holds all related information relating to the response (out-bound).
 */
class QTTPSHARED_EXPORT HttpResponse
{
  friend class HttpServer;
  friend class HttpData;

  private:

    HttpResponse(native::http::QttpResponse* resp);

  public:

    ~HttpResponse();

    // TODO: PASS A ENUM TO TO HELP REDUCE CASE SENSITIVE ISSUES!
    // TODO: Should go ahead and accept a whole container as well.
    void setHeader(const QString& key, const QString& value);
    void setHeader(const QList<QStringPair>& headers);

    void setStatus(HttpStatus status);
    HttpStatus getStatus() const;

    /**
     * @brief Build your HTTP response by populating your data into QJsonObject.
     *
     * This is the preferred way to build/access a json response - if it is
     * valid, the QttpServer framework (namely httpserver) will take this object
     * and populate the HTTP response body automatically.
     *
     * As an alternative, the caller may optionally complete the transaction
     * with finishResponse().
     */
    QJsonObject& getJson();
    const QJsonObject& getJson() const;
    void setJson(const QJsonObject& json);

    /**
     * @brief Preferred method when working with the json object.  Populate
     * response data using getJson() and then invoke this method to send it off.
     */
    bool finish();

    /**
     * @brief A wrapper for native::http::response::end() and writes directly
     * to the response socket.  For direct writes, this is highly encouraged
     * since it helps track the state of the response - i.e. if the response was
     * written to the socket already.
     * @see isFinished()
     */
    bool finish(const std::string& body);
    bool finish(const QByteArray& bytes);
    bool finish(const QJsonObject& json);

    /**
     * @return Boolean indicating if finishResponse() has been called.
     *
     * It's important to actually avoid using any references to
     * native::http::request and native::http::response if this returns true.
     */
    bool isFinished() const;

    /**
     * @return If this was processed by any processor or action.  Intended to
     * track if this data object was operated on already.
     */
    bool isProcessed() const;

    bool shouldContinue() const;

    bool isTerminated() const;
    void setTerminated();
    inline void terminate()
    {
      setTerminated();
    }

  protected:

    quint32 getFlag() const;
    void setFlag(DataControl flag);

  public:

    /**
     * @brief Returns the response object, note that response::end() method is
     * ABSOLUTELY DISCOURAGED from direct invocation since the we have no way of
     * tracking the state of the response socket.
     *
     * Similar to getRequest(), this method will raise an exception if
     * finishResponse() has been invoked since memory too will be invalid.
     *
     * DO NOT to save/use this reference outside of the lifetime of HttpData.
     */
    const native::http::QttpResponse* getResponse();

  private:

    QTTP_DECLARE_ASSERT_MEMBER(native::http::QttpResponse)

    native::http::QttpResponse * m_Response;
    HttpStatus m_Status;
    QJsonObject m_Json;
    quint32 m_ControlFlag;
};

}
#endif // QTTPHTTPRESPONSE_H
