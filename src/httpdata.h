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
 * @todo Add a friend stream operator for pretty-printing
 *
 * @brief
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
    HttpData(native::http::request*, native::http::response*);
    ~HttpData();

  public:

    typedef enum eControl
    {
      // Provided for Processors to abort/terminate processing.
      Terminated = 0x0001,
      // Set internally to indicate that the response was sent.
      Finished = 0x0002,
      // Set internally to indicate that preprocessors operated on this.
      Preprocessed = 0x0004,
      // Set internally to indicate that postprocessors operated on this.
      Postprocessed = 0x0008,
      // Set internally to indicate this was operated on by an Action.
      ActionProcessed = 0x0010,
      // Default value.
      None = 0x0000

    } eControl;

    /**
     * @brief Beware that accessing this after invoking finishResponse() will
     * result in an exception since alloated memory will be NULL.  Also be sure
     * NOT to save/use this reference outside of the lifetime of HttpData.
     */
    native::http::request& getRequest() const;

    /**
     * @brief Recommended for qt-centric applications that deal with QStrings.
     * This will save the user from having to constantly convert to a QString.
     */
    const HttpRequest& getHttpRequest() const;
    HttpRequest& getHttpRequest();

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
    native::http::response& getResponse() const;

    /**
     * @brief Recommended for qt-centric applications that deal with QStrings.
     * This will save the user from having to constantly convert to a QString.
     */
    const HttpResponse& getHttpResponse() const;
    HttpResponse& getHttpResponse();

    /**
     * @brief Highly recommended!  This builds and returns a QJsonObject that
     * consolidates query-string parameters and the reqeust body (JSON only).
     * This avoids having to grab from the query string and parsing through the
     * request body separately.
     */
    QJsonObject& getRequestParams();

    QUrlQuery& getQuery();

    const QUrlQuery& getQuery() const;

    const QUuid& getUid() const;

    /**
     * @brief A wrapper for native::http::response::end() and writes directly
     * to the response socket.  For direct writes, this is highly encouraged
     * since it helps track the state of the response - i.e. if the response was
     * written to the socket already.
     * @see isFinished()
     */
    bool finishResponse(const std::string& body);

    /**
     * @brief The preferred way to build/access a json response - complete the
     * transaction with finishResponse().
     */
    QJsonObject& getJson();

    const QJsonObject& getJson() const;

    /**
     * @brief Preferred method when working with the json object.  Populate
     * response data using getJson() and then invoke this method to send it off.
     */
    bool finishResponse();

    bool finishResponse(const QJsonObject& json);

    quint32 getControlFlag() const;

    bool shouldContinue() const;

    void setTerminated();

    bool isTerminated() const;

    /**
     * @return Boolean indicating if finishResponse() has been called.
     *
     * It's important to actually avoid using any references to
     * native::http::request and native::http::response if this returns true.
     */
    bool isFinished() const;

    // Not sure how useful these are yet - YAGNI?  Ya ain't gunna need it.
    // bool isPreprocessed() const;
    // bool isPostprocessed() const;
    // bool isActionProcessed() const;

    /**
     * @return If this was processed by any processor or action.  Intended to
     * track if this data object was operated on already.
     */
    bool isProcessed() const;

    void setTimestamp(const QDateTime& timestamp);

    const QDateTime& getTimestamp() const;

    const QTime& getTime() const;

  private:

    /**
     * @brief Sets a flag to indicate if the action should continue processing
     * or simply marks this data as having been processed e.g. preprocess was
     * performed already.
     */
    void setControlFlag(eControl flag);

    /**
     * @brief This will swap data - beware
     */
    void setQuery(QUrlQuery&);

    native::http::request* m_Request;
    mutable QSharedPointer<HttpRequest> m_HttpRequest;
    native::http::response* m_Response;
    mutable QSharedPointer<HttpResponse> m_HttpResponse;
    QUrlQuery m_Query;
    QJsonObject m_Json;
    QJsonObject m_RequestParams;
    quint32 m_ControlFlag;
    QUuid m_Uid;
    QDateTime m_Timestamp;
    QTime m_Time;
};

}

#endif // QTTPHTTPDATA_H
