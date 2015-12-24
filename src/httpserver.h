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
#include "httpevent.h"
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
    void setEventCallback(std::function<void(HttpEvent*)> eventCallback);

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
    bool addAction(const QString&, std::function<void(HttpData& data)>);

    /**
     * @brief More of an association than a registration - binds an action name
     * to a route url.
     */
    bool registerRoute(const QString& type, const QString& actionName, const QString& route);

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

    Stats& getStats();

    LoggingUtils& getLoggingUtils();

    bool isInitialized() const
    {
      return m_IsInitialized;
    }

    bool initialize(QCoreApplication* app = nullptr);

    QCommandLineParser& getCommandLineParser();

  private:

    void registerRouteFromJSON(QJsonValueRef& obj, const QString& method);

    /**
     * @brief defaultCallback
     * @param req
     * @param resp
     */
    std::function<void(HttpEvent*)> defaultEventCallback() const;

    void performPreprocessing(HttpData& data) const;

    void performPostprocessing(HttpData& data) const;

    /**
     * @brief Initial entry point for all incomming http requests from libuv.
     * @param object
     * @param event
     * @return
     */
    bool eventFilter(QObject* object, QEvent* event);

    /**
     * @brief Determines if "path" matches the pattern "pathParts".
     * @param pathParts The route or pattern to match
     * @param path The requested endpoint
     * @param responseParams The list of parameters parsed from "path"
     * @return If there is a match
     */
    static bool matchUrl(const QStringList& pathParts, const QString& path, QUrlQuery& responseParams);

    static HttpServer* m_Instance;

    /**
     * @todo Copy constructor and the move constructor!
     * @brief The Route class
     */
    class Route
    {
      public:
        Route() :
          route(),
          action(),
          parts()
        {
        }

        Route(const QString& r, const QString& a) :
          route(r),
          action(a),
          parts(route.split('/', QString::SkipEmptyParts))
        {
        }

      QString route;
      QString action;
      QStringList parts;
    };

    /// @brief Private constructor per singleton design.
    HttpServer();
    // TODO make the copy-constructor private as well.

    /// @brief This callback allows the caller to intercept all responses.
    std::function<void(HttpEvent*)> m_EventCallback;
    QHash<QString, std::shared_ptr<Action>> m_Actions;
    QHash<QString, std::function<void(HttpData& data)>> m_ActionCallbacks;
    QHash<QString, Route> m_GetRoutes;
    QHash<QString, Route> m_PostRoutes;
    QHash<QString, Route> m_PutRoutes;
    QHash<QString, Route> m_DeleteRoutes;
    QHash<QString, Route> m_PatchRoutes;
    std::vector<std::shared_ptr<Processor>> m_Processors;
    std::vector<std::function<void(HttpData& data)>> m_Preprocessors;
    std::vector<std::function<void(HttpData& data)>> m_Postprocessors;
    QJsonObject m_GlobalConfig;
    QJsonObject m_RoutesConfig;
    Stats* m_Stats; //! To work around const captures this is a pointer.
    LoggingUtils m_LoggingUtils;
    bool m_IsInitialized;
    QCommandLineParser m_CmdLineParser;
    bool m_SendRequestMetadata;
};

} // End namespace qttp

#endif // QTTPHTTPSERVER_H
