#ifndef QTTPHTTPSERVER_H
#define QTTPHTTPSERVER_H

#include "qttp_global.h"
#include "httproute.h"
#include "action.h"
#include "httpdata.h"
#include "httpevent.h"
#include "fileutils.h"

#ifdef QTTP_COLLECT_STATS
  #define STATS_INC(X) m_Stats->increment( X )
  #define STATS_SET(X, Y) m_Stats->setValue( X, Y )
#else
  #define STATS_INC(X)
  #define STATS_SET(X, Y)
#endif

namespace qttp
{

class QTTPSHARED_EXPORT HttpServer : public QObject
{
  Q_OBJECT

  public:

    static const char* SERVE_FILES_PATH;
    static const char* GLOBAL_CONFIG_FILE;
    static const char* GLOBAL_CONFIG_FILE_PATH;
    static const char* ROUTES_CONFIG_FILE;
    static const char* ROUTES_CONFIG_FILE_PATH;
    static const char* CONFIG_DIRECTORY_ENV_VAR;
    static const char* QTTP_HOME_ENV_VAR;

    static HttpServer* getInstance();
    virtual ~HttpServer();

    /**
     * @brief Kick off the libuv event loop.
     * @return The integer result from node.native's run method.
     */
    void startServer();
    void startServer(QString ip, int port);

    static int start();
    static void stop();

    /**
     * @brief By default the HttpServer will exit if it can't successfully bind
     * to the desired ip/port.  This allows the caller to override the the
     * default behavior.
     * @param serverErrorCallback
     */
    void setServerErrorCallback(std::function<void()> serverErrorCallback);

    /**
     * @brief setEventFilter
     * @param callback
     */
    void setEventCallback(std::function<void(HttpEvent*)> eventCallback);

    //! Publicly available in case people are too lazy to generate their own.
    QString generateActionName() const;

    /**
     * @brief A template method to register an action via the Action interface.
     */
    template<class T> std::shared_ptr<Action> addAction()
    {
      std::shared_ptr<Action> action(new T());
      if(!HttpServer::addAction(action))
      {
        LOG_DEBUG("A previously installed action has been replaced");
      }
      return action;
    }

    /**
     * @brief A template method to register an action via the Action interface.
     * This special option offers a special mechanism to pass plain-old-data
     * from the main thread to potentially reduce the need for singletons and
     * the like.
     */
    template<class T, class P> std::shared_ptr<Action> addAction(P& param)
    {
      std::shared_ptr<Action> action(new T(param));
      if(!HttpServer::addAction(action))
      {
        LOG_DEBUG("A previously installed action has been replaced");
      }
      return action;
    }

    /**
     * @brief Takes ownership of the pointer passed in, do not delete!
     * @return boolean true if a previous route was replaced, false otherwise
     */
    bool addAction(std::shared_ptr<Action>& action);

    /**
     * @brief Encouraged for those who need a quick and easy way to setup an
     * action.
     */
    std::shared_ptr<Action> createAction(std::function<void(HttpData& data)>);
    std::shared_ptr<Action> createAction(const QString&, std::function<void(HttpData& data)>);

    //! You are highly encouraged to use the register route options below!
    bool registerRoute(const QString& method, const QString& actionName, const QString& path, Visibility visibilty = Visibility::Show);
    bool registerRoute(HttpMethod method, const QString& actionName, const QString& path, Visibility visibilty = Visibility::Show);

    /**
     * @brief More of an association than a registration - binds an action name
     * to a route url.
     */
    bool registerRoute(std::shared_ptr<Action> action, HttpMethod method, const QString& path, Visibility visibility = Visibility::Show);
    bool registerRoute(std::shared_ptr<Action> action, const qttp::HttpPath& path, Visibility visibility = Visibility::Show);
    bool registerRoute(HttpMethod method, const Route& route);


    template<class T> std::shared_ptr<Action> addActionAndRegister(Visibility visibilty = Visibility::Show)
    {
      std::shared_ptr<Action> action(new T());
      HttpServer::addAction(action);
      auto routes = action->getRoutes();
      for(const auto & path : routes)
      {
        HttpServer::registerRoute(action, path, visibilty);
      }
      return action;
    }

    template<class T> std::shared_ptr<Action> addActionAndRegister(const QString& path,
                                                                   const std::initializer_list<HttpMethod>& methods,
                                                                   Visibility visibilty = Visibility::Show)
    {
      std::shared_ptr<Action> action(new T());
      HttpServer::addAction(action);
      for(HttpMethod method : methods)
      {
        HttpServer::registerRoute(action, method, path, visibilty);
      }
      return action;
    }

    template<class T> std::shared_ptr<Action> addActionAndRegister(const QString& path,
                                                                   const std::initializer_list<QString>& methods,
                                                                   Visibility visibilty = Visibility::Show)
    {
      std::shared_ptr<Action> action(new T());
      HttpServer::addAction(action);
      for(auto & method : methods)
      {
        HttpServer::registerRoute(action, Utils::fromString(method), path, visibilty);
      }
      return action;
    }

    /**
     * @brief A template method to register a processor via the Processor interface.
     */
    template<class T> bool addProcessor()
    {
      std::shared_ptr<Processor> processor(new T());
      return HttpServer::addProcessor(processor);
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

    bool isSwaggerEnabled() const
    {
      return m_IsSwaggerEnabled;
    }

    bool initialize();
    void initGlobal(const QString& filepath);
    void initRoutes(const QString& filepath);
    void initConfigDirectory(const QString& path);
    void initHttpDirectory(const QString& path);

    QCommandLineParser& getCommandLineParser();

    class QTTPSHARED_EXPORT ServerInfo
    {
      public:
        ServerInfo()
        {
        }
        QString title;
        QString description;
        QString version;
        QString termsOfService;
        QString contactEmail;
        QString licenseName;
        QString licenseUrl;
        QString companyName;
        QString companyUrl;
        QString host;
        QString basePath;
        QJsonArray schemes;
        QJsonArray consumes;
        QJsonArray produces;
    };

    const Action* getAction(const QString& name) const;

    std::shared_ptr<Action> getAction(const QString& name);

    const QHash<QString, Route>& getRoutes(HttpMethod method) const;

    const QHash<QString, Route>& getRoutes(const QString& method) const;

    const QJsonObject& getGlobalConfig() const;

    const QJsonObject& getRoutesConfig() const;

    const QHash<QString, std::shared_ptr<const Action> >& getActions() const;

    const ServerInfo& getServerInfo() const;

    void setServerInfo(const HttpServer::ServerInfo& serverInfo);

QTTP_PRIVATE:

    template<class T> bool addDefaultProcessor()
    {
      std::shared_ptr<Processor> processor(new T());
      return HttpServer::addDefaultProcessor(processor);
    }

    bool addDefaultProcessor(std::shared_ptr<Processor>& processor);

    void registerRouteFromJSON(QJsonValueRef& obj, const QString& method);

    void registerRouteFromJSON(QJsonValueRef& obj, HttpMethod method);

    /**
     * @brief defaultCallback
     * @param req
     * @param resp
     */
    std::function<void(HttpEvent*)> defaultEventCallback() const;

    void performPreprocessing(HttpData& data) const;

    void performPostprocessing(HttpData& data) const;

    bool searchAndServeFile(HttpData& data) const;

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

    /// @brief Private constructor per singleton design.
    HttpServer();

    //! Forbid copy constructor
    HttpServer(const HttpServer&) = delete;
    void operator =(const HttpServer&) {
    }

    static HttpServer* m_Instance;
    static const char* SERVER_ERROR_MSG;

    std::function<void()> m_ServerErrorCallback;
    /// @brief This callback allows the caller to intercept all responses.
    std::function<void(HttpEvent*)> m_EventCallback;
    QHash<QString, std::shared_ptr<Action> > m_Actions;
    QHash<QString, std::shared_ptr<const Action> > m_ConstActions;
    QMap<HttpMethod, QHash<QString, Route> > m_Routes;
    std::vector<std::shared_ptr<Processor> > m_Processors;
    std::vector<std::function<void(HttpData& data)> > m_Preprocessors;
    std::vector<std::function<void(HttpData& data)> > m_Postprocessors;
    QJsonObject m_GlobalConfig;
    QJsonObject m_RoutesConfig;
    Stats* m_Stats; //! To work around const captures this is a pointer.
    LoggingUtils m_LoggingUtils;
    bool m_IsInitialized;
    bool m_IsSwaggerEnabled;
    QCommandLineParser m_CmdLineParser;
    bool m_SendRequestMetadata;
    bool m_StrictHttpMethod;
    bool m_ShouldServeFiles;
    QDir m_ServeFilesDirectory;
    FileUtils m_FileLookup;
    QStringList m_EnabledProcessors;
    ServerInfo m_ServerInfo;
};

} // End namespace qttp

#endif // QTTPHTTPSERVER_H
