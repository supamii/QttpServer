#ifndef QTTPHTTPDATA_H
#define QTTPHTTPDATA_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>

namespace qttp
{

// Forward declaration
class HttpServer;

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
    native::http::request& getRequest() const;

    /**
     * @brief Returns the response object, note that response::end() method is
     * ABSOLUTELY DISCOURAGED from direct invocation since the we have no way of
     * tracking the state of the response socket.
     */
    native::http::response& getResponse() const;

    /**
     * @brief A wrapper for native::http::response::end() and writes directly
     * to the response socket.  For direct writes, this is highly encouraged
     * since it helps track the state of the response - i.e. if the response was
     * written to the socket already.
     * @see isFinished()
     */
    bool finishResponse(const std::string&);

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

    /**
     * @return Boolean indicating if finishResponse() has been called.
     */
    bool isFinished() const;

  private:
    native::http::request* m_Request;
    native::http::response* m_Response;
    QJsonObject m_Json;
    bool m_IsFinished;
};

}

#endif // QTTPHTTPDATA_H
