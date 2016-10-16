#include "httpserver.h"
#include "httpevent.h"
#include "swagger.h"
#include "defaults.h"

using namespace std;
using namespace qttp;
using namespace native::http;

HttpServer* HttpServer::m_Instance = nullptr;
const char* HttpServer::SERVE_FILES_PATH = "/usr/local/qttpfiles";
const char* HttpServer::GLOBAL_CONFIG_FILE = "global.json";
const char* HttpServer::ROUTES_CONFIG_FILE = "routes.json";
const char* HttpServer::GLOBAL_CONFIG_FILE_PATH = "./config/global.json";
const char* HttpServer::ROUTES_CONFIG_FILE_PATH = "./config/routes.json";
const char* HttpServer::CONFIG_DIRECTORY_ENV_VAR = "QTTP_CONFIG_DIRECTORY";
const char* HttpServer::SERVER_ERROR_MSG = "Unable to bind to ip/port, exiting...";

HttpServer* HttpServer::getInstance()
{
  if(m_Instance == nullptr)
  {
    m_Instance = new HttpServer();
  }
  return m_Instance;
}

HttpServer::HttpServer() :
  QObject(),
  m_ServerErrorCallback([](){
  LOG_FATAL(SERVER_ERROR_MSG);
}),
  m_EventCallback(this->defaultEventCallback()),
  m_Actions(),
  m_ConstActions(),
  m_ActionCallbacks(),
  m_Routes(),
  m_Processors(),
  m_Preprocessors(),
  m_Postprocessors(),
  m_GlobalConfig(),
  m_RoutesConfig(),
  m_Stats(new Stats()),
  m_LoggingUtils(),
  m_IsInitialized(false),
  m_CmdLineParser(),
  m_SendRequestMetadata(false),
  m_StrictHttpMethod(false),
  m_ShouldServeFiles(true),
  m_IsSwaggerEnabled(false),
  m_ServeFilesDirectory(SERVE_FILES_PATH),
  m_Thread(HttpServer::start),
  m_EnabledProcessors()
{
  this->installEventFilter(this);

  for(auto method = Global::HTTP_METHODS.begin();
      method != Global::HTTP_METHODS.end();
      ++method)
  {
    m_Routes[*method] = QHash<QString, Route>();
  }
}

HttpServer::~HttpServer()
{
  if(m_Stats)
  {
    delete m_Stats;
  }
}

bool HttpServer::initialize()
{
  if(m_IsInitialized)
  {
    LOG_DEBUG("Already initialized");
    return false;
  }

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  if(env.contains("QTTP_HOME"))
  {
    QDir::setCurrent(env.value("QTTP_HOME"));
    LOG_DEBUG("Working directory" << QDir::currentPath());
  }
  else
  {
    // Just a quirk for mac, but I wonder if we can apply to all in general.
    #ifdef Q_OS_MAC
    QDir::setCurrent(qApp->applicationDirPath());
    LOG_DEBUG("Working directory" << QDir::currentPath());
    #else
    LOG_DEBUG("Working directory" << QDir::currentPath());
    #endif
  }

  QCoreApplication* app = QCoreApplication::instance();
  Q_ASSERT(app);

  m_CmdLineParser.addOptions({
    {{"i", "ip"},
     QCoreApplication::translate("main", "ip of the target interface"),
     QCoreApplication::translate("main", "ip")},
    {{"p", "port"},
     QCoreApplication::translate("main", "port to listen on"),
     QCoreApplication::translate("main", "port")},
    {{"m", "meta"},
     QCoreApplication::translate("main", "appends metadata to responses")},
    {{"c", "config"},
     QCoreApplication::translate("main", "absolute path to the global config file (json)"),
     QCoreApplication::translate("main", "config")},
    {{"r", "routes"},
     QCoreApplication::translate("main", "absolute path to the routes config file (json)"),
     QCoreApplication::translate("main", "routes")},
    {{"d", "dir"},
     QCoreApplication::translate("main", "absolute path to the config directory, don't combine with -c or -r args"),
     QCoreApplication::translate("main", "dir")}
  });

  m_CmdLineParser.addHelpOption();
  m_CmdLineParser.process(*app);

  QJsonValue i = m_CmdLineParser.value("i");
  if((i.isString() || i.isDouble()) && !i.toString().trimmed().isEmpty())
  {
    QString ip = i.toString();
    m_GlobalConfig["bindIp"] = ip;
    LOG_DEBUG("Cmd line ip" << ip);
  }

  QJsonValue p = m_CmdLineParser.value("p");
  if((p.isString() || p.isDouble()) && !p.toString().trimmed().isEmpty())
  {
    qint32 port = p.toInt();
    m_GlobalConfig["bindPort"] = port;
    LOG_DEBUG("Cmd line port" << port);
  }

  if(env.contains(CONFIG_DIRECTORY_ENV_VAR))
  {
    QString var = env.value(CONFIG_DIRECTORY_ENV_VAR);
    if(!var.isNull() && !var.trimmed().isEmpty())
    {
      LOG_INFO("Processing ENVIRONMENT VARIABLE [" << var << "]");
      initConfigDirectory(var);
    }
    else
    {
      LOG_WARN("Invalid ENVIRONMENT VARIABLE [" << CONFIG_DIRECTORY_ENV_VAR << "]");
    }
  }

  QJsonValue d = m_CmdLineParser.value("d");
  if(d.isString() && !d.isNull() && !d.toString().trimmed().isEmpty())
  {
    initConfigDirectory(d.toString());
  }
  else
  {
    QJsonValue c = m_CmdLineParser.value("c");
    if(c.isString() && !c.isNull() && !c.toString().trimmed().isEmpty())
    {
      initGlobal(c.toString());
    }
    else
    {
      initGlobal(GLOBAL_CONFIG_FILE_PATH);
    }

    QJsonValue r = m_CmdLineParser.value("r");
    if(r.isString() && !r.isNull() && !r.toString().trimmed().isEmpty())
    {
      initRoutes(r.toString());
    }
    else
    {
      initRoutes(ROUTES_CONFIG_FILE_PATH);
    }
  }

  if(!m_SendRequestMetadata)
  {
    m_SendRequestMetadata = m_CmdLineParser.isSet("m");
  }

  m_IsInitialized = true;

  return true;
}

void HttpServer::initGlobal(const QString &filepath)
{
  LOG_INFO("Processing filepath [" << filepath << "]");

  m_GlobalConfig = Utils::readJson(QDir(filepath).absolutePath());

  QJsonValueRef loggingValue = m_GlobalConfig["logfile"];
  if(loggingValue.isObject())
  {
    QJsonObject logging = loggingValue.toObject();
    if(logging["isEnabled"].toBool(true))
    {
      QString filename;
      if(logging["filename"].isString())
      {
        filename = logging["filename"].toString();
      }
      if(logging["writeFrequency"].isDouble())
      {
        m_LoggingUtils.initializeFile(filename, logging["writeFrequency"].toInt());
      }
      else
      {
        m_LoggingUtils.initializeFile(filename);
      }
    }
  }

  QJsonValueRef httpFilesValue = m_GlobalConfig["httpFiles"];
  if(httpFilesValue.isObject())
  {
    QJsonObject httpFiles = httpFilesValue.toObject();
    m_ShouldServeFiles = httpFiles["isEnabled"].toBool(false);
    if(m_ShouldServeFiles)
    {
      m_ServeFilesDirectory = httpFiles["directory"].toString();
      if(!m_ServeFilesDirectory.exists())
      {
        LOG_ERROR("Unable to serve files from invalid directory [" << m_ServeFilesDirectory.absolutePath() << "]");
        m_ShouldServeFiles = false;
      }
    }
  }

  QJsonObject swagger = m_GlobalConfig["swagger"].toObject();
  m_IsSwaggerEnabled = swagger["isEnabled"].toBool(false);

  QJsonObject headers = m_GlobalConfig["defaultHeaders"].toObject();
  QStringList keys = headers.keys();

  if(!keys.isEmpty())
  {
    Global::DEFAULT_HEADERS.clear();
    for(QString key : keys)
    {
      QString value = headers.value(key).toString();
      Global::DEFAULT_HEADERS.append({ key.toStdString(), value.toStdString() });
      LOG_DEBUG("Adding default-header [" << key << ", " << value << "]");
    }

    // We'll always force the QttpServer version in here.
    Global::DEFAULT_HEADERS.append({ "Server", QTTP_SERVER_VERSION });
  }
  else
  {
    LOG_DEBUG("Did not read headers in config file, using default headers");
  }

  QJsonObject serverConfig = m_GlobalConfig["server"].toObject();
  m_SendRequestMetadata = serverConfig["metadata"].toBool(false);
  m_StrictHttpMethod = serverConfig["strictHttpMethod"].toBool(false);

  QJsonObject processors = serverConfig["processors"].toObject();
  keys = processors.keys();
  for(QString key : keys)
  {
    bool isEnabled = processors.value(key).toBool(false);

    LOG_DEBUG("Processor [" << key << "] is " <<
              (isEnabled ? "ENABLED" : "NOT ENABLED"));

    if(isEnabled)
    {
      m_EnabledProcessors.append(key);
    }
  }
}

void HttpServer::initRoutes(const QString &filepath)
{
  LOG_INFO("Processing filepath [" << filepath << "]");

  m_RoutesConfig = Utils::readJson(QDir(filepath).absolutePath());

  QJsonValueRef get = m_RoutesConfig["get"];
  registerRouteFromJSON(get, "get");

  QJsonValueRef post = m_RoutesConfig["post"];
  registerRouteFromJSON(post, "post");

  QJsonValueRef put = m_RoutesConfig["put"];
  registerRouteFromJSON(put, "put");

  QJsonValueRef patch = m_RoutesConfig["patch"];
  registerRouteFromJSON(patch, "patch");

  QJsonValueRef head = m_RoutesConfig["head"];
  registerRouteFromJSON(head, "head");

  // This only really here because of an older bug - doh.  Should remove later.
  QJsonValueRef del = m_RoutesConfig["del"];
  registerRouteFromJSON(del, "delete");

  QJsonValueRef delRoute = m_RoutesConfig["delete"];
  registerRouteFromJSON(delRoute, "delete");

  QJsonValueRef options = m_RoutesConfig["options"];
  registerRouteFromJSON(options, "options");

  QJsonValueRef trace = m_RoutesConfig["trace"];
  registerRouteFromJSON(trace, "trace");

  QJsonValueRef conn = m_RoutesConfig["connect"];
  registerRouteFromJSON(conn, "connect");
}

void HttpServer::initConfigDirectory(const QString &path)
{
  LOG_INFO("Processing directory [" << path << "]");
  QDir dir = path;
  initGlobal(dir.filePath(GLOBAL_CONFIG_FILE));
  initRoutes(dir.filePath(ROUTES_CONFIG_FILE));
}

void HttpServer::registerRouteFromJSON(QJsonValueRef& obj, const QString& method)
{
  return registerRouteFromJSON(obj, Utils::fromString(method.toUpper()));
}

void HttpServer::registerRouteFromJSON(QJsonValueRef& obj, HttpMethod method)
{
  if(obj.isArray())
  {
    QJsonArray array = obj.toArray();
    auto item = array.begin();
    while(item != array.end())
    {
      if(item->isObject())
      {
        auto route = item->toObject();
        auto action = route["action"].toString().trimmed();
        auto path = route["path"].toString().trimmed();
        if(route["isActive"] != false && !path.isEmpty())
        {
          this->registerRoute(method, action, path);
        }
      }
      ++item;
    }
  }
}

void HttpServer::startServer()
{
  HttpServer& svr = *(HttpServer::getInstance());

  if(svr.m_IsSwaggerEnabled)
  {
    svr.addActionAndRegister<Swagger>();
  }

  svr.addDefaultProcessor<OptionsPreprocessor>();

  auto quitCB = [](){
                  LOG_TRACE;
                  HttpServer::getInstance()->stop();
                };

  QObject::connect(QCoreApplication::instance(),
                   &QCoreApplication::aboutToQuit,
                   quitCB);

  // thread.detach() invokes HttpServer::start()
  svr.m_Thread.detach();
}

int HttpServer::start()
{
  HttpServer& svr = *(HttpServer::getInstance());

  QString ip = svr.m_GlobalConfig["bindIp"].toString("0.0.0.0").trimmed();
  auto port = svr.m_GlobalConfig["bindPort"].toInt(8080);

  auto serverCB = [](request& req, response& resp) {
                    HttpEvent* event = new HttpEvent(&req, &resp);
                    QCoreApplication::postEvent(HttpServer::getInstance(), event);
                  };

  native::http::http server;
  auto result = server.listen(ip.toStdString(), port, serverCB);

  if(!result)
  {
    LOG_ERROR("Unable to bind to" << ip << port);

    if(svr.m_ServerErrorCallback)
    {
      svr.m_ServerErrorCallback();
    }

    return 1;
  }

  LOG_INFO("Server pid" << QCoreApplication::applicationPid() <<
           "running at" << ip << port);

  return native::run();
}

void HttpServer::stop()
{
  native::stop();
}

void HttpServer::setServerErrorCallback(function<void()> serverErrorCallback)
{
  m_ServerErrorCallback = serverErrorCallback;
}

void HttpServer::setEventCallback(function<void(HttpEvent*)> eventCallback)
{
  m_EventCallback = eventCallback;
}

function<void(HttpEvent*)> HttpServer::defaultEventCallback() const
{
  // TODO: Can benefit performance gains by caching look up costs - don't care
  // about amortized theoretical values.
  //
  // TODO: Perhaps should lock/wrap m_Routes to guarantee atomicity.

  return [&](HttpEvent * event) mutable
         {
           STATS_INC("http:hits");

           HttpData data(event->getRequest(), event->getResponse());
           data.setTimestamp(event->getTimestamp());

           HttpResponse& response = data.getResponse();
           HttpRequest& request = data.getRequest();

           HttpMethod method = request.getMethod(m_StrictHttpMethod);
           switch(method)
           {
             case HttpMethod::GET:
               STATS_INC("http:method:get");
               break;

             case HttpMethod::POST:
               STATS_INC("http:method:post");
               break;

             case HttpMethod::PUT:
               STATS_INC("http:method:put");
               break;

             case HttpMethod::PATCH:
               STATS_INC("http:method:patch");
               break;

             case HttpMethod::HEAD:
               STATS_INC("http:method:head");
               break;

             case HttpMethod::DELETE:
               STATS_INC("http:method:delete");
               break;

             case HttpMethod::OPTIONS:
               STATS_INC("http:method:options");
               break;

             case HttpMethod::CONNECT:
               STATS_INC("http:method:connect");
               break;

             case HttpMethod::TRACE:
               STATS_INC("http:method:trace");
               break;

             default:
               STATS_INC("http:method:unknown");
               response.setStatus(HttpStatus::BAD_REQUEST);
               QJsonObject& json = data.getResponse().getJson();
               json["error"] = "Invalid HTTP method";
               return;
           }

           auto routes = m_Routes.find(method);

           // Err on the side of caution since this ptr may be null.
           if(routes == m_Routes.end())
           {
             LOG_ERROR("Invalid route");
             response.setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
             QJsonObject& json = data.getResponse().getJson();
             json["error"] = "Internal error";
             return;
           }

           QUrlQuery parameters;
           const QString& urlPath = request.getUrl().getPath();
           auto route = routes->begin();

           // TODO: ROOM FOR IMPROVEMENT: We can reduce the total number of searches
           // for the worst case scenario.

           for(; route != routes->end(); ++route)
           {
             parameters.clear();

             if(HttpServer::matchUrl(route.value().parts, urlPath, parameters))
             {
               // Since this request is going to be processed, let's also parse the
               // query strings for easy access.
               QUrlQuery params(request.getUrl().getQuery());
               for(auto i : params.queryItems())
               {
                 // Should note that existing itms are not replaced!  These are simply
                 // appended to the query string.
                 parameters.addQueryItem(i.first, i.second);
               }
               request.setQuery(parameters);
               break;
             }
           }

           try
           {
             // Previously we had hard-routes that weren't dynamic.
             // auto route = routes->find(urlPath);

             if(route != routes->end())
             {
               auto callback = m_ActionCallbacks.find(route.value().action);
               if(callback != m_ActionCallbacks.end())
               {
                 performPreprocessing(data);
                 if(response.shouldContinue())
                 {
                   response.setFlag(DataControl::ActionProcessed);
                   (callback.value())(data);
                 }
                 if(response.shouldContinue()) performPostprocessing(data);
               }
               else
               {
                 auto action = m_Actions.find(route.value().action);
                 if(action != m_Actions.end() && action.value().get() != nullptr)
                 {
                   performPreprocessing(data);
                   if(response.shouldContinue())
                   {
                     response.setFlag(DataControl::ActionProcessed);
                     auto a = action.value();
                     a->applyHeaders(data);
                     a->onAction(data);
                   }
                   if(response.shouldContinue()) performPostprocessing(data);
                 }
               }
             }

             // Check the control flag bit mask to determine if it was processed above.
             if(!response.isProcessed())
             {
               LOG_DEBUG("No route found for" << urlPath << ", "
                         "checking default routes");

               // Even if the action is not yet found, we'll give the user a chance to
               // intercept and process it.
               performPreprocessing(data);

               if(response.shouldContinue())
               {
                 // TODO: Describe this in the header file.

                 // TODO: Can also perform this check once in a while instead to reduce
                 // performance lookup costs.

                 // Actions registered under "" is the default handler.
                 auto callback = m_ActionCallbacks.find("");
                 if(callback != m_ActionCallbacks.end())
                 {
                   response.setFlag(DataControl::ActionProcessed);
                   callback.value()(data);
                   if(response.shouldContinue()) performPostprocessing(data);
                 }
                 else
                 {
                   auto action = m_Actions.find("");
                   if(action != m_Actions.end())
                   {
                     response.setFlag(DataControl::ActionProcessed);
                     auto a = action.value();
                     a->applyHeaders(data);
                     a->onAction(data);
                     if(response.shouldContinue()) performPostprocessing(data);
                   }
                   else
                   {
                     // Check out files as a last resort.
                     if(!searchAndServeFile(data))
                     {
                       response.setStatus(HttpStatus::BAD_REQUEST);
                       QJsonObject& json = data.getResponse().getJson();
                       json["error"] = "Invalid request";
                       performPostprocessing(data);
                     }
                   }
                 }
               } // End if(data.shouldContinue())
             } // End if(!data.isProcessed())
           }
           catch(const std::exception& e)
           {
             LOG_ERROR("Exception caught" << e.what());
             response.setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
             QJsonObject& json = data.getResponse().getJson();
             json["error"] = e.what();
           }
           catch(const QJsonObject& e)
           {
             LOG_ERROR("JSON caught" << e);
             response.setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
             data.getResponse().getJson() = e;
           }
           catch(...)
           {
             response.setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
             QJsonObject& json = data.getResponse().getJson();
             json["error"] = "Internal server error";
           }

           if(!response.isFinished())
           {
             if(!data.getResponse().getJson().isEmpty())
             {
               if(m_SendRequestMetadata)
               {
                 QJsonObject obj;
                 bool ip4;
                 std::string ip;
                 int port;

                 if(response.getResponse()->getpeername(ip4, ip, port))
                 {
                   obj["remoteIp"] = ip.c_str();
                   obj["remotePort"] = port;
                 }

                 obj["query"] = request.getQuery().toString();
                 obj["uid"] = data.getUid().toString();
                 obj["timestamp"] = data.getTimestamp().toString("yyyy/MM/dd hh:mm:ss:zzz");
                 obj["timeElapsed"] = data.getTime().elapsed();
                 obj["timeElapsedMs"] = (qreal)(uv_hrtime() - request.getTimestamp()) /
                                        (qreal)1000000.00;

                 QJsonObject& json = data.getResponse().getJson();
                 json["requestMetadata"] = obj;
               }

               if( !response.finish())
               {
                 LOG_WARN("Failed to finish response");
               }
             }
           }
         };
}

bool HttpServer::matchUrl(const QStringList& routeParts, const QString& path, QUrlQuery& params)
{
  // Using splitRef to reduce string copies.
  QVector<QStringRef> urlParts = path.splitRef('/', QString::SkipEmptyParts);

  if(urlParts.length() != routeParts.length())
  {
    return false;
  }

  QString variable = "";
  QString regexp = "";

  for(int i = 0; i < urlParts.length(); ++i)
  {
    const QStringRef& urlPart = urlParts[i];
    const QString& routePart = routeParts[i];

    if(routePart.startsWith(':') && routePart.indexOf('(') < 0)
    {
      // We found something like /:id/ so let's make sure we grab that.
      variable = QString(routePart).replace(':', "");
      params.addQueryItem(variable, urlPart.toString());
    }
    else if(routePart.startsWith(':') && routePart.indexOf('(') >= 0)
    {
      // Proceed to grab the regular expression inside the expressed route.
      // TODO: TEST THIS
      QString urlPartTmp = urlPart.toString();
      regexp = routePart.mid(routePart.indexOf('(') + 1, routePart.length() - 1);
      bool matches = urlPartTmp.contains(QRegularExpression(regexp));
      if(matches)
      {
        variable = QString(routePart).replace(':', "").split('(')[0];
        params.addQueryItem(variable, urlPartTmp);
      }
      else
      {
        return false;
      }
    }
    else if(urlPart.isEmpty() || routePart != urlPart)
    {
      return false;
    }
  }

  LOG_DEBUG("Found route" << path);
  return true;
}

void HttpServer::performPreprocessing(HttpData& data) const
{
  auto& response = data.getResponse();
  for(auto & callback : m_Preprocessors)
  {
    callback(data);
    response.setFlag(DataControl::Preprocessed);
  }

  for(auto & processor : m_Processors)
  {
    if(processor.get())
    {
      processor->preprocess(data);
      response.setFlag(DataControl::Preprocessed);
    }
  }
}

void HttpServer::performPostprocessing(HttpData& data) const
{
  auto& response = data.getResponse();
  auto processor = m_Processors.rbegin();
  auto end = m_Processors.rend();

  while(processor != end)
  {
    Processor* p = processor->get();
    if(p)
    {
      p->postprocess(data);
      response.setFlag(DataControl::Postprocessed);
    }
    ++processor;
  }

  for(auto & callback : m_Postprocessors)
  {
    callback(data);
    response.setFlag(DataControl::Postprocessed);
  }
}

bool HttpServer::searchAndServeFile(HttpData& data) const
{
  if(!m_ShouldServeFiles)
  {
    return false;
  }

  LOG_TRACE;

  // TODO: WOULD BE NICE TO CACHE STRING CONSTRUCTION.

  //QString urlFragment = data.getHttpRequest().getUrl().getFragment();
  QString urlPath = data.getRequest().getUrl().getPath();
  if(urlPath.at(0) == '/')
  {
    urlPath = urlPath.mid(1);
  }
  QString filepath = m_ServeFilesDirectory.absoluteFilePath(urlPath);
  QFile file(filepath);

  if(!file.open(QIODevice::ReadOnly))
  {
    LOG_DEBUG("Unable to read file [" << filepath << "]");
    return false;
  }

  string contentType = "text/html";

  if(urlPath.endsWith(".js", Qt::CaseInsensitive))
  {
    contentType = "text/javascript";
  }
  else if(urlPath.endsWith(".html", Qt::CaseInsensitive) ||
          urlPath.endsWith(".htm", Qt::CaseInsensitive))
  {
    // No-op. Meant to help reduce constantly checking for other types.
  }
  else if(urlPath.endsWith(".json", Qt::CaseInsensitive))
  {
    contentType = "application/javascript";
  }
  else if(urlPath.endsWith(".css", Qt::CaseInsensitive) ||
          urlPath.endsWith(".less", Qt::CaseInsensitive))
  {
    contentType = "text/css";
  }
  else if(urlPath.endsWith(".png", Qt::CaseInsensitive) ||
          urlPath.endsWith(".gif", Qt::CaseInsensitive) ||
          urlPath.endsWith(".jpg", Qt::CaseInsensitive) ||
          urlPath.endsWith(".jpeg", Qt::CaseInsensitive) ||
          urlPath.endsWith(".ico", Qt::CaseInsensitive))
  {
    contentType = "image";
  }
  else if(urlPath.endsWith(".svg", Qt::CaseInsensitive))
  {
    contentType = "image/svg+xml";
  }
  else if(urlPath.endsWith(".xml", Qt::CaseInsensitive))
  {
    contentType = "application/xml";
  }

  auto& response = data.getResponse();
  response.setHeader("Content-Type", contentType);
  return response.finish(file.readAll());
}

bool HttpServer::eventFilter(QObject* /* object */, QEvent* event)
{
  if(!event || event->type() != QEvent::None)
  {
    return false;
  }

  HttpEvent* httpEvent = dynamic_cast<HttpEvent*>(event);
  if(httpEvent == nullptr)
  {
    return false;
  }

  if(!httpEvent->getRequest() || !httpEvent->getResponse())
  {
    LOG_ERROR("Request or response is NULL");
    return false;
  }

  m_EventCallback(httpEvent);
  return true;
}

bool HttpServer::addAction(std::shared_ptr<Action>& action)
{
  bool containsKey = (m_Actions.find(action->getName()) != m_Actions.end());
  QString name = action->getName();
  m_Actions[name] = action;
  m_ConstActions[name] = action;
  // Let the caller know that we kicked out another action handler.
  return !containsKey;
}

bool HttpServer::addAction(const QString& actionName, function<void(HttpData& data)> callback)
{
  bool containsKey = (m_ActionCallbacks.find(actionName) != m_ActionCallbacks.end());
  m_ActionCallbacks[actionName] = callback;
  return !containsKey;
}

const Action* HttpServer::getAction(const QString& name) const
{
  auto action = m_Actions.find(name);
  if(action != m_Actions.end())
  {
    return action.value().get();
  }
  return nullptr;
}

std::shared_ptr<Action> HttpServer::getAction(const QString& name)
{
  auto action = m_Actions.find(name);
  if(action != m_Actions.end())
  {
    return action.value();
  }
  return nullptr;
}

const QHash<QString, HttpServer::Route>& HttpServer::getRoutes(HttpMethod method) const
{
  auto route = m_Routes.find(method);
  if(route == m_Routes.end())
  {
    THROW_EXCEPTION("Invalid route method [" << (int) method << "]");
  }
  return route.value();
}

const QHash<QString, HttpServer::Route>& HttpServer::getRoutes(const QString& method) const
{
  if(m_StrictHttpMethod)
  {
    return getRoutes(Utils::fromString(method));
  }
  return getRoutes(Utils::fromPartialString(method));
}

const QJsonObject& HttpServer::getGlobalConfig() const
{
  return m_GlobalConfig;
}

const QJsonObject& HttpServer::getRoutesConfig() const
{
  return m_RoutesConfig;
}

const QHash<QString, std::shared_ptr<const Action> >& HttpServer::getActions() const
{
  return m_ConstActions;
}

bool HttpServer::registerRoute(const QString& method, const QString& actionName, const QString& route)
{
  return registerRoute(Utils::fromString(method.toUpper()), actionName, route);
}

bool HttpServer::registerRoute(HttpMethod method, const QString& actionName, const QString& route)
{
  auto routes = m_Routes.find(method);
  if(routes == m_Routes.end())
  {
    LOG_ERROR("Invalid http "
              "action [" << actionName << "] "
              "route [" << route << "]");
    return false;
  }

  LOG_DEBUG("method [" << Utils::toString(method) << "] "
            "action [" << actionName << "] "
            "route [" << route << "]");

  bool containsKey = (routes->find(route) != routes->end());

  // Initialize and assign the Route struct.
  routes->insert(route, Route(route, actionName));

  return !containsKey;
}

bool HttpServer::addProcessor(std::shared_ptr<Processor>& processor)
{
  if(processor.get() == nullptr)
  {
    return false;
  }

  LOG_DEBUG("Adding processor [" << processor->getName() << "]");
  m_Processors.push_back(processor);
  return true;
}

bool HttpServer::addDefaultProcessor(std::shared_ptr<Processor>& processor)
{
  if(processor.get() == nullptr)
  {
    return false;
  }

  if(!m_EnabledProcessors.contains(processor->getName()))
  {
    LOG_DEBUG("Processor not enabled [" << processor->getName() << "]");
    return false;
  }

  return addProcessor(processor);
}

void HttpServer::addPreprocessor(std::function<void(HttpData& data)> callback)
{
  m_Preprocessors.push_back(callback);
}

void HttpServer::addPostprocessor(std::function<void(HttpData& data)> callback)
{
  m_Postprocessors.push_back(callback);
}

Stats& HttpServer::getStats()
{
  Q_ASSERT(m_Stats);
  return *m_Stats;
}

LoggingUtils& HttpServer::getLoggingUtils()
{
  return m_LoggingUtils;
}

QCommandLineParser& HttpServer::getCommandLineParser()
{
  return m_CmdLineParser;
}
