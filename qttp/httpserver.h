#ifndef QTTPHTTPSERVER_H
#define QTTPHTTPSERVER_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>
#include <functional>
#include <vector>
#include <unordered_map>

#include "action.h"
#include "httpdata.h"
#include "utils.h"

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

    /**
     * @brief A template method to register an action via the Action interface.
     */
    template<class T> bool addAction()
    {
      std::shared_ptr<Action> action(new T());
      // TODO: May need to add a dynamic cast check?
      return addAction(action);
    }

    /**
     * @brief Takes ownership of the pointer passed in, do not delete.
     */
    bool addAction(std::shared_ptr<Action>& action);

    /**
     * @brief Encouraged for those who need a quick and easy way to setup an
     * action.
     * @return boolean false if a previous route was replaced, false otherwise
     */
    bool addAction(const std::string&, std::function<void(HttpData& data)>);

    bool registerRoute(const std::string&, const std::string&);

    /**
     * @brief initialize
     */
    void initialize();

    template<class T> bool addProcessor()
    {
      std::shared_ptr<Processor> processor(new T());
      return addProcessor(processor);
    }

    bool addProcessor(std::shared_ptr<Processor>& processor);

  private:

    /**
     * @brief defaultCallback
     * @param req
     * @param resp
     */
    std::function<void(native::http::request*, native::http::response*)> defaultEventCallback() const;

    void preprocessAction(HttpData& data) const;

    void postprocessAction(HttpData& data) const;

    /**
     * @brief Initial entry point for all incomming http requests from libuv.
     * @param object
     * @param event
     * @return
     */
    bool eventFilter(QObject *object, QEvent *event);

    static std::unique_ptr<HttpServer> m_Instance;

    /// @brief Private constructor per singleton design.
    HttpServer();
    /// @brief This callback allows the caller to intercept all responses.
    std::function<void(native::http::request*, native::http::response*)> m_EventCallback;
    std::unordered_map<std::string, std::shared_ptr<Action>> m_Actions;
    std::unordered_map<std::string, std::function<void(HttpData& data)>> m_ActionCallbacks;
    std::unordered_map<std::string, std::string> m_Routes;
    std::vector<std::shared_ptr<Processor>> m_Processors;
    QJsonObject m_GlobalConfig;
    QJsonObject m_RoutesConfig;
};

} // End namespace qttp

#endif // QTTPHTTPSERVER_H
