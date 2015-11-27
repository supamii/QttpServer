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
    native::http::response& getResponse() const;
    QJsonObject& getJson();
    const QJsonObject& getJson() const;

  private:
    native::http::request* m_Request;
    native::http::response* m_Response;
    QJsonObject m_Json;
};

}

#endif // QTTPHTTPDATA_H
