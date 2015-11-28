#ifndef QTTPHTTPDATA_H
#define QTTPHTTPDATA_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>

namespace qttp
{

class HttpData
{
  public:
    /// @brief Throws an exception if arguments are NULL.
    HttpData(native::http::request*, native::http::response*);
    virtual ~HttpData();

    native::http::request& getRequest() const;

    /**
     * @brief Returns the response object, note that response::end() method is
     * ABSOLUTELY DISCOURAGED from direct invocation since the we have no way of
     * tracking the state of the response socket.
     */
    native::http::response& getResponse() const;

    QJsonObject& getJson();
    const QJsonObject& getJson() const;

    /**
     * @brief A wrapper for response::end().  This is highly encouraged as it
     * helps track the state of the response - i.e. if the response was written
     * to the socket already.  This enables isFinished().
     */
    bool finishResponse(const std::string&);

    /**
     * @brief Preferred method when working with the json object.  TBD.
     */
    bool finishJsonResponse();

    bool isFinished() const;

  private:
    native::http::request* m_Request;
    native::http::response* m_Response;
    QJsonObject m_Json;
    bool m_IsFinished;
};

}

#endif // QTTPHTTPDATA_H
