#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>
#include <functional>

namespace qttp
{

class HttpServer : public QObject
{
  Q_OBJECT

  public:
    static HttpServer* getInstance();
    virtual ~HttpServer();

    /**
     * @brief A statically accessible function to kick off the libuv event loop.
     * @return The integer result from node.native's run method.
     */
    static int start();

    /**
     * @brief setEventFilter
     * @param callback
     */
    void setEventCallback(std::function<void(native::http::request*, native::http::response*)> eventCallback);

  protected:

    /**
     * @brief defaultCallback
     * @param req
     * @param resp
     */
    std::function<void(native::http::request*, native::http::response*)> defaultCallback() const;

    /**
     * @brief Initial entry point for all incomming http requests from libuv.
     * @param object
     * @param event
     * @return
     */
    bool eventFilter(QObject *object, QEvent *event);

  private:

    /**
     * @brief Private constructor per singleton design.
     */
    HttpServer();
    static std::unique_ptr<HttpServer> m_Instance;
    std::function<void(native::http::request*, native::http::response*)> m_EventCallback;
};

} // End namespace qttp

#endif // HTTPSERVER_H
