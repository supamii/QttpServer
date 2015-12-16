#ifndef QTTPHTTPSERVER_H
#define QTTPHTTPSERVER_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>
#include <functional>
#include <vector>
#include <unordered_map>

#include "../src/action.h"
#include "../src/httpdata.h"
#include "../src/utils.h"

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
     * @brief Takes ownership of the pointer passed in, do not delete!
     */
    bool addAction(std::shared_ptr<Action>& action);

    /**
     * @brief Encouraged for those who need a quick and easy way to setup an
     * action.
     * @return boolean false if a previous route was replaced, false otherwise
     */
    bool addAction(const std::string&, std::function<void(HttpData& data)>);

    /**
     * @brief More of an association than a registration - binds an action name
     * to a route name.
     */
    bool registerRoute(const std::string& actionName, const std::string& routeName);

    /**
     * @brief A template method to register a processor via the Processor interface.
     */
    template<class T> bool addProcessor()
    {
      std::shared_ptr<Processor> processor(new T());
      return addProcessor(processor);
    }

    /**
     * @brief Takes ownership of the pointer passed in, do not delete!
     *
     * Pre-processor actions are performend in order and the post-processing
     * is executed in the "reverse" order that the processor was added.
     *
     * May need to add some priority-queue (value) to help coordinate order.
     */
    bool addProcessor(std::shared_ptr<Processor>& processor);

    /**
     * @brief A quick way to add a preprocessor to operate on all actions.
     *
     * Pre-processors are executed in the order that they were added.
     */
    void addPreprocessor(std::function<void(HttpData& data)> callback);

    /**
     * @brief A quick way to add a postprocessor to operate on all actions.
     *
     * Post-processors are executed in the order that they were added.
     */
    void addPostprocessor(std::function<void(HttpData& data)> callback);

  private:

    void initialize();

    /**
     * @brief defaultCallback
     * @param req
     * @param resp
     */
    std::function<void(native::http::request*, native::http::response*)> defaultEventCallback() const;

    void performPreprocessing(HttpData& data) const;

    void performPostprocessing(HttpData& data) const;

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
    std::vector<std::function<void(HttpData& data)>> m_Preprocessors;
    std::vector<std::function<void(HttpData& data)>> m_Postprocessors;
    QJsonObject m_GlobalConfig;
    QJsonObject m_RoutesConfig;
};

} // End namespace qttp

#endif // QTTPHTTPSERVER_H
