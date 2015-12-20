#ifndef QTTPHTTPDATA_H
#define QTTPHTTPDATA_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>

namespace qttp
{

// Forward declaration
class HttpServer;

/**
 * @brief
 */
class HttpData
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

    /**
     * @brief Beware that accessing this after invoking finishResponse() will
     * result in an exception since alloated memory will be NULL.  Also be sure
     * NOT to save/use this reference outside of the lifetime of HttpData.
     */
    native::http::request& getRequest() const;

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

    QUrlQuery& getQuery();

    const QUrlQuery& getQuery() const;

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

    /**
     * @return Boolean indicating if finishResponse() has been called.
     *
     * It's important to actually avoid using any references to
     * native::http::request and native::http::response if this returns true.
     */
    bool isFinished() const;

    /// @brief Sets a flag to indicate if the action should continue processing.
    void setControlFlag(bool shouldContinue);

    bool getControlFlag() const;

  private:

    /**
     * @brief This will swap data - beware
     */
    void setQuery(QUrlQuery&);


    native::http::request* m_Request;
    native::http::response* m_Response;
    QUrlQuery m_Query;
    QJsonObject m_Json;
    /// TODO Consolidate with control flag.
    bool m_IsFinished;

    /// TODO Create enum flags.
    bool m_ControlFlag;
};

}

#endif // QTTPHTTPDATA_H
