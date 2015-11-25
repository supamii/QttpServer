#ifndef QTTPHTTPSERVER_H
#define QTTPHTTPSERVER_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>
#include <functional>
#include <vector>
#include <unordered_map>

#include "action.h"

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
      T* t = new T();
      bool containsKey = (m_Actions.find(t->getActionName()) != m_Actions.end());
      m_Actions[t->getActionName()] = std::shared_ptr<Action>(t);
      // Let the caller know that we kicked out another action handler.
      return !containsKey;
    }

    /**
     * @brief Takes ownership of the pointer passed in, do not delete.
     */
    template<class T> bool addAction(std::shared_ptr<Action>& action)
    {
      bool containsKey = (m_Actions.find(action->getActionName()) != m_Actions.end());
      m_Actions[action->getActionName()] = action;
      // Let the caller know that we kicked out another action handler.
      return !containsKey;
    }

    /**
     * @brief Encouraged for those who need a quick and easy way to setup an
     * action.
     * @return boolean false if a previous route was replaced, false otherwise
     */
    bool addAction(const std::string&, std::function<void(native::http::request*, native::http::response*)>);

    bool registerRoute(const std::string&, const std::string&);

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

    /// @brief Private constructor per singleton design.
    HttpServer();
    static std::unique_ptr<HttpServer> m_Instance;
    /// @brief This callback allows the caller to intercept all responses.
    std::function<void(native::http::request*, native::http::response*)> m_EventCallback;
    std::unordered_map<std::string, std::shared_ptr<Action>> m_Actions;
    std::unordered_map<std::string, std::function<void(native::http::request*, native::http::response*)>> m_ActionCallbacks;
    std::unordered_map<std::string, std::string> m_Routes;
    // TODO: We could optimize by reducing a double-lookup on routes to actions;
};

} // End namespace qttp

#endif // QTTPHTTPSERVER_H
