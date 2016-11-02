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
const char* HttpServer::QTTP_HOME_ENV_VAR = "QTTP_HOME";
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
  m_ServerErrorCallback(),
  m_EventCallback(this->defaultEventCallback()),
  m_Actions(),
  m_ConstActions(),
  m_Routes(),
  m_Processors(),
  m_Preprocessors(),
  m_Postprocessors(),
  m_GlobalConfig(),
  m_RoutesConfig(),
  m_Stats(new Stats()),
  m_LoggingUtils(),
  m_IsInitialized(false),
  m_IsSwaggerEnabled(false),
  m_CmdLineParser(),
  m_SendRequestMetadata(false),
  m_StrictHttpMethod(false),
  m_ShouldServeFiles(true),
  m_ServeFilesDirectory(SERVE_FILES_PATH),
  m_FileLookup(),
  m_EnabledProcessors(),
  m_ServerInfo()
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
  if(env.contains(QTTP_HOME_ENV_VAR))
  {
    QDir::setCurrent(env.value(QTTP_HOME_ENV_VAR));
    LOG_DEBUG("Working directory from $" << QTTP_HOME_ENV_VAR << QDir::currentPath());
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

  LOG_INFO(m_GlobalConfig["bindIp"]);
  LOG_INFO(m_GlobalConfig["bindPort"]);

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
      QString directory = httpFiles["directory"].toString().trimmed();
      if(directory == "$QTTP_HOME")
      {
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        if(env.contains(QTTP_HOME_ENV_VAR))
        {
          m_ServeFilesDirectory = QDir::cleanPath(env.value(QTTP_HOME_ENV_VAR));
          m_ServeFilesDirectory = m_ServeFilesDirectory.absoluteFilePath("www");
          LOG_DEBUG("Using $QTTP_HOME" << m_ServeFilesDirectory.absolutePath());
        }
        else
        {
          m_ServeFilesDirectory = QDir::current().absoluteFilePath("www");
          LOG_DEBUG("QTTP_HOME not found, using current directory" << m_ServeFilesDirectory.absolutePath());
        }
      }
      else if(directory.isEmpty())
      {
        m_ServeFilesDirectory = QDir::current().absoluteFilePath("www");
        LOG_DEBUG("Default to using current directory" << m_ServeFilesDirectory.absolutePath());
      }
      else
      {
        m_ServeFilesDirectory = QDir::cleanPath(directory);
        LOG_DEBUG("Using directory in config" << m_ServeFilesDirectory.absolutePath());
      }
      if(!m_ServeFilesDirectory.exists())
      {
        LOG_ERROR("Unable to serve files from invalid directory [" << m_ServeFilesDirectory.absolutePath() << "]");
        m_ShouldServeFiles = false;
      }
    }
  }

  if(m_ShouldServeFiles)
  {
    m_FileLookup.populateFiles(m_ServeFilesDirectory);
  }

  QJsonObject headers = m_GlobalConfig["defaultHeaders"].toObject();
  QStringList keys = headers.keys();

  if(!keys.isEmpty())
  {
    Global::DEFAULT_HEADERS.clear();
    for(QString key : keys)
    {
      QString value = headers.value(key).toString();
      Global::DEFAULT_HEADERS.push_back({ key, value });
      LOG_DEBUG("Adding default-header [" << key << ", " << value << "]");
    }

    // We'll always force the QttpServer version in here.
    Global::DEFAULT_HEADERS.push_back({ "Server", QTTP_SERVER_VERSION });
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

  QJsonObject swagger = m_GlobalConfig["swagger"].toObject();
  m_IsSwaggerEnabled = swagger["isEnabled"].toBool(false);

#ifndef ASSIGN_SWAGGER
#  define ASSIGN_SWAGER(X) m_ServerInfo.X = swagger[#X].toString()
#endif

  ASSIGN_SWAGER(host);
  ASSIGN_SWAGER(basePath);
  ASSIGN_SWAGER(version);
  ASSIGN_SWAGER(title);
  ASSIGN_SWAGER(description);
  ASSIGN_SWAGER(termsOfService);

  QJsonObject company = swagger["company"].toObject();
  m_ServerInfo.companyName = company["name"].toString();
  m_ServerInfo.companyUrl = company["url"].toString();

  QJsonObject contact = swagger["contact"].toObject();
  m_ServerInfo.contactEmail = contact["email"].toString();

  QJsonObject license = swagger["license"].toObject();
  m_ServerInfo.licenseName = license["name"].toString();
  m_ServerInfo.licenseUrl = license["url"].toString();

  m_ServerInfo.schemes = swagger["schemes"].toArray();
  m_ServerInfo.consumes = swagger["consumes"].toArray();
  m_ServerInfo.produces = swagger["produces"].toArray();
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
  HttpServer* svr = HttpServer::getInstance();

  if(svr->m_IsSwaggerEnabled)
  {
    svr->addActionAndRegister<Swagger>();
  }

  svr->addDefaultProcessor<OptionsPreprocessor>();

  auto quitCB = [svr](){
                  LOG_TRACE;
                  svr->stop();
                };

  QObject::connect(QCoreApplication::instance(),
                   &QCoreApplication::aboutToQuit,
                   quitCB);

  std::thread newThread(HttpServer::start);
  newThread.detach();
}

int HttpServer::start()
{
  HttpServer* svr = HttpServer::getInstance();

  QString ip = svr->m_GlobalConfig["bindIp"].toString("0.0.0.0").trimmed();
  auto port = svr->m_GlobalConfig["bindPort"].toInt(8080);

  auto callback = [svr](QttpRequest& req, QttpResponse& resp) {
                    HttpEvent* event = new HttpEvent(&req, &resp);
                    QCoreApplication::postEvent(svr, event);
                  };

  native::http::Qttp server;
  auto result = server.listen(ip.toStdString(), port, callback);

  if(!result)
  {
    LOG_ERROR("Unable to bind to" << ip << ":" << port);

    if(svr->m_ServerErrorCallback)
    {
      svr->m_ServerErrorCallback();
    }

    stringstream ss;
    ss << ip.toStdString() << ":" << port << " " << SERVER_ERROR_MSG;
    LOG_FATAL(ss.str().c_str());
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

             case HttpMethod::DEL:
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
               json["error"] = QSTR("Invalid HTTP method");
               return;
           }

           auto routes = m_Routes.find(method);

           // Err on the side of caution since this ptr may be null.
           if(routes == m_Routes.end())
           {
             LOG_ERROR("Invalid route");
             response.setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
             QJsonObject& json = data.getResponse().getJson();
             json["error"] = QSTR("Internal error");
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
               for(auto & i : params.queryItems())
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
             if(route != routes->end())
             {
               auto action = m_Actions.find(route.value().action);
               if(action != m_Actions.end() && action.value().get() != nullptr)
               {
                 performPreprocessing(data);
                 if(response.shouldContinue())
                 {
                   response.setFlag(DataControl::ActionProcessed);
                   auto& a = action.value();
                   a->applyHeaders(data);
                   a->onAction(data);
                 }
                 if(response.shouldContinue()) performPostprocessing(data);
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

                 auto action = m_Actions.find("");
                 if(action != m_Actions.end())
                 {
                   response.setFlag(DataControl::ActionProcessed);
                   auto& a = action.value();
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
                     json["error"] = QSTR("Invalid request");
                     performPostprocessing(data);
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
             json["error"] = QSTR("Internal server error");
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

  LOG_DEBUG("Found path" << path);
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

  QString urlPath = data.getRequest().getUrl().getPath();
  if(urlPath.startsWith('/'))
  {
    urlPath = urlPath.mid(1);
  }

  QString filepath = QDir::cleanPath(m_ServeFilesDirectory.absoluteFilePath(urlPath));
  QFile file(filepath);

  // NOTE:
  // Won't need this check when/if we support multiple directories.
  if(!filepath.startsWith(m_ServeFilesDirectory.absolutePath()))
  {
    LOG_ERROR("Not allowed to read from path [" << filepath << "]");
    return false;
  }

  QDir directory = filepath;

  if(m_FileLookup.hasDir(filepath) &&
     directory.exists() &&
     QFile::exists(filepath + "/index.html"))
  {
    filepath += "/index.html";
    file.setFileName(filepath);
    LOG_DEBUG("Detected index.html [" << filepath << "]");
  }

  if(!m_FileLookup.hasFile(filepath))
  {
    LOG_ERROR("File not in lookup list [" << filepath << "]");
    return false;
  }

  if(!file.open(QIODevice::ReadOnly))
  {
    LOG_DEBUG("Unable to read file [" << filepath << "]");
    return false;
  }

  LOG_DEBUG("Serving file [" << filepath << "]");

  QString contentType = FileUtils::determineContentType(urlPath);

  // TODO: -- Move this into FileUtils::LoadFile
  // where we can manage opening and closing.

  auto& response = data.getResponse();
  response.setHeader("Content-Type", contentType);
  auto result = response.finish(file.readAll());
  file.close();
  return result;
}

bool HttpServer::eventFilter(QObject* object, QEvent* event)
{
  Q_UNUSED(object);

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

QString HttpServer::generateActionName() const
{
  int sanityLimit = 10;
  QString uuid;

  do
  {
    uuid = QUuid::createUuid().toString();
    if((--sanityLimit) < 1)
    {
      LOG_FATAL("UNABLE TO CREATE A UNIQUE ID WHILE ADDING AN ACTION");
    }

    // A bit extreme but just in case... let's regenerate UUIDs if there is
    // already one!?

  } while(m_Actions.contains(uuid));

  return uuid;
}

std::shared_ptr<Action> HttpServer::createAction(std::function<void(HttpData&)> callback)
{
  QString uuid = generateActionName();
  return createAction(uuid, callback);
}

std::shared_ptr<Action> HttpServer::createAction(const QString& actionName, std::function<void(HttpData&)> callback)
{
  std::shared_ptr<Action> action(new SimpleAction(callback));
  SimpleAction* simpleAction = reinterpret_cast<SimpleAction*>(action.get());
  simpleAction->m_Name = std::move(actionName.toUtf8());
  if(!HttpServer::addAction(action))
  {
    LOG_DEBUG("A previously installed action has been replaced");
  }
  return action;
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

const QHash<QString, Route>& HttpServer::getRoutes(HttpMethod method) const
{
  auto route = m_Routes.find(method);
  if(route == m_Routes.end())
  {
    THROW_EXCEPTION("Invalid route method [" << (int) method << "]");
  }
  return route.value();
}

const QHash<QString, Route>& HttpServer::getRoutes(const QString& method) const
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

void HttpServer::setServerInfo(const HttpServer::ServerInfo& serverInfo)
{
  m_ServerInfo = serverInfo;
}

const HttpServer::ServerInfo& HttpServer::getServerInfo() const
{
  return m_ServerInfo;
}

bool HttpServer::registerRoute(const QString& method, const QString& actionName, const QString& path, Visibility visibility)
{
  return registerRoute(Utils::fromString(method.trimmed().toUpper()), actionName, path, visibility);
}

bool HttpServer::registerRoute(HttpMethod method, const QString& action, const QString& path, Visibility visibility)
{
  return registerRoute(method, Route(action, path, visibility));
}

bool HttpServer::registerRoute(std::shared_ptr<Action> action, HttpMethod method, const QString& path, Visibility visibility)
{
  return registerRoute(action, { method, path }, visibility);
}

bool HttpServer::registerRoute(std::shared_ptr<Action> action, const qttp::HttpPath& path, Visibility visibility)
{
  return registerRoute(path.first, Route(action->getName(), path.second, visibility));
}

bool HttpServer::registerRoute(HttpMethod method, const Route& route)
{
  auto routes = m_Routes.find(method);
  if(routes == m_Routes.end())
  {
    LOG_ERROR("Invalid http "
              "action [" << route.action << "] "
              "path [" << route.path << "]");
    return false;
  }

  LOG_DEBUG("method [" << Utils::toString(method) << "] "
            "action [" << route.action << "] "
            "path [" << route.path << "]");

  bool containsKey = (routes->find(route.path) != routes->end());

  // Initialize and assign the Route struct.
  routes->insert(route.path, route);

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
