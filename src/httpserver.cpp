#include "httpserver.h"

#include "httpevent.h"

using namespace std;
using namespace qttp;
using namespace native::http;

HttpServer* HttpServer::m_Instance = nullptr;

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
    m_EventCallback(this->defaultEventCallback()),
    m_Actions(),
    m_ActionCallbacks(),
    m_GetRoutes(),
    m_PostRoutes(),
    m_PutRoutes(),
    m_DeleteRoutes(),
    m_PatchRoutes(),
    m_Processors(),
    m_Preprocessors(),
    m_Postprocessors(),
    m_GlobalConfig(),
    m_RoutesConfig(),
    m_Stats(new Stats()),
    m_LoggingUtils(),
    m_IsInitialized(false)
{
  this->installEventFilter(this);
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

  m_GlobalConfig = Utils::readJson(QDir("config/global.json").absolutePath());

  QJsonValue loggingObj = m_GlobalConfig["logfile"];
  if(loggingObj.isObject())
  {
    QJsonObject logging = loggingObj.toObject();
    if(logging["isEnabled"].isBool() && logging["isEnabled"].toBool())
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

  m_RoutesConfig = Utils::readJson(QDir("config/routes.json").absolutePath());

  QJsonValueRef get = m_RoutesConfig["get"];
  registerRouteFromJSON(get, "get");

  QJsonValueRef post = m_RoutesConfig["post"];
  registerRouteFromJSON(post, "post");

  QJsonValueRef put = m_RoutesConfig["put"];
  registerRouteFromJSON(put, "put");

  QJsonValueRef del = m_RoutesConfig["del"];
  registerRouteFromJSON(del, "del");

  m_IsInitialized = true;

  return true;
}

void HttpServer::registerRouteFromJSON(QJsonValueRef& obj, const QString& method)
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

int HttpServer::start()
{
  HttpServer& svr = *(HttpServer::getInstance());
  QString ip = svr.m_GlobalConfig["bindIp"].isUndefined() ? "0.0.0.0" : svr.m_GlobalConfig["bindIp"].toString().trimmed();
  if(ip.isEmpty())
  {
    ip = "0.0.0.0";
    LOG_WARN("Bind ip is invalid, defaulting to" << ip);
  }
  auto port = svr.m_GlobalConfig["bindPort"].isUndefined() ? 8080 : svr.m_GlobalConfig["bindPort"].toInt();
  if(port <= 0)
  {
    port = 8080;
    LOG_WARN("Bind port is invalid, defaulting to" << port);
  }
  native::http::http server;
  auto result = server.listen(ip.toStdString(), port, [](request& req, response& resp) {
    HttpEvent* event = new HttpEvent(&req, &resp);
    QCoreApplication::postEvent(HttpServer::getInstance(), event);
  });

  if(!result)
  {
    LOG_ERROR("Unable to bind to" << ip << port);
    return 1;
  }

  LOG_INFO("Server running at" << ip << port);
  return native::run();
}

void HttpServer::setEventCallback(function<void(request*, response*)> eventCallback)
{
  m_EventCallback = eventCallback;
}

function<void(request*, response*)> HttpServer::defaultEventCallback() const
{
  // TODO: Can benefit performance gains by caching look up costs - don't care
  // about amortized theoretical values.

  // TODO: Perhaps should lock/wrap m_Routes to guarantee atomicity.

  return [&](request* req, response* resp) mutable
  {
    m_Stats->increment("http:hits");

    HttpData data(req, resp);

    const QHash<QString, Route>* routes = &m_GetRoutes;
    QString method = QString(req->get_method().c_str()).toUpper().trimmed();

    if(method == "GET")
    {
      m_Stats->increment("http:method:get");
      routes = &m_GetRoutes;
    }
    else if(method == "POST")
    {
      m_Stats->increment("http:method:post");
      routes = &m_PostRoutes;
    }
    else if(method == "PUT")
    {
      m_Stats->increment("http:method:put");
      routes = &m_PutRoutes;
    }
    else if(method == "DELETE")
    {
      m_Stats->increment("http:method:delete");
      routes = &m_DeleteRoutes;
    }
    else
    {
      m_Stats->increment("http:method:unknown");
    }

    QUrlQuery parameters;
    QString urlPath = QString::fromStdString(req->url().path()).trimmed();
    auto route = routes->begin();

    // TODO: ROOM FOR IMPROVEMENT: We can reduce the total number of searches
    // for the worst case scenario.

    for(; route != routes->end(); ++route)
    {
      parameters.clear();

      if(HttpServer::matchUrl(route.value().parts,
                              urlPath,
                              parameters))
      {
        // Since this request is going to be processed, let's also parse the
        // query strings for easy access.
        QUrlQuery params(QString::fromStdString(req->url().query()));
        for(auto i : params.queryItems())
        {
          // Should note that existing itms are not replaced!  These are simply
          // appended to the query string.
          parameters.addQueryItem(i.first, i.second);
        }
        data.setQuery(parameters);
        break;
      }
    }

    // Previously we had hard-routes that weren't dynamic.
    // auto route = routes->find(urlPath);

    if(route != routes->end())
    {
      auto callback = m_ActionCallbacks.find(route.value().action);
      if(callback != m_ActionCallbacks.end())
      {
        performPreprocessing(data);
        if(data.shouldContinue())
        {
          data.setControlFlag(HttpData::ActionProcessed);
          (callback.value())(data);
        }
        if(data.shouldContinue()) performPostprocessing(data);
      }
      else
      {
        auto action = m_Actions.find(route.value().action);
        if(action != m_Actions.end() && action.value().get() != nullptr)
        {
          performPreprocessing(data);
          if(data.shouldContinue())
          {
            data.setControlFlag(HttpData::ActionProcessed);
            (action.value())->onAction(data);
          }
          if(data.shouldContinue()) performPostprocessing(data);
        }
      }
    }

    // Check the control flag bit mask to determine if it was processed above.
    if(!data.isProcessed())
    {
      LOG_DEBUG("No route found for" << urlPath << ", checking default routes");

      // Even if the action is not yet found, we'll give the user a chance to
      // intercept and process it.
      performPreprocessing(data);

      if(data.shouldContinue())
      {
        // TODO: Describe this in the header file.

        // TODO: Can also perform this check once in a while instead to reduce
        // performance lookup costs.

        // Actions registered under "" is the default handler.
        auto callback = m_ActionCallbacks.find("");
        if(callback != m_ActionCallbacks.end())
        {
          data.setControlFlag(HttpData::ActionProcessed);
          callback.value()(data);
          if(data.shouldContinue()) performPostprocessing(data);
        }
        else
        {
          auto action = m_Actions.find("");
          if(action != m_Actions.end())
          {
            data.setControlFlag(HttpData::ActionProcessed);
            (action.value())->onAction(data);
            if(data.shouldContinue()) performPostprocessing(data);
          }
          else
          {
            resp->set_status(400);
            QJsonObject& json = data.getJson();
            json["error"] = "Invalid request";

            performPostprocessing(data);
          }
        }
      }
    }

    if(!data.getJson().isEmpty() && !data.isFinished() && !data.finishResponse())
    {
      LOG_WARN("Failed to finish response");
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
  for(auto& callback : m_Preprocessors)
  {
    callback(data);
    data.setControlFlag(HttpData::Preprocessed);
  }

  for(auto& processor : m_Processors)
  {
    if(processor.get())
    {
      processor->preprocess(data);
      data.setControlFlag(HttpData::Preprocessed);
    }
  }
}

void HttpServer::performPostprocessing(HttpData& data) const
{
  auto processor = m_Processors.rbegin();
  auto end = m_Processors.rend();

  while(processor != end)
  {
    Processor* p = processor->get();
    if(p)
    {
      p->postprocess(data);
      data.setControlFlag(HttpData::Postprocessed);
    }
    ++processor;
  }

  for(auto& callback : m_Postprocessors)
  {
    callback(data);
    data.setControlFlag(HttpData::Postprocessed);
  }
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

  request* req = httpEvent->getData().first;
  response* resp = httpEvent->getData().second;

  if(!req || !resp)
  {
    LOG_WARN("Request or response is NULL");
  }

  m_EventCallback(req, resp);
  return true;
}

bool HttpServer::addAction(std::shared_ptr<Action>& action)
{
  bool containsKey = (m_Actions.find(action->getActionName()) != m_Actions.end());
  m_Actions[action->getActionName()] = action;
  // Let the caller know that we kicked out another action handler.
  return !containsKey;
}

bool HttpServer::addAction(const QString& actionName, function<void(HttpData& data)> callback)
{
  bool containsKey = (m_ActionCallbacks.find(actionName) != m_ActionCallbacks.end());
  m_ActionCallbacks[actionName] = callback;
  return !containsKey;
}

bool HttpServer::registerRoute(const QString& method, const QString& actionName, const QString& route)
{
  QHash<QString, Route>* routeContainer = nullptr;
  QString methodStr = method.trimmed().toLower();
  if(methodStr == "get")
  {
    routeContainer = &m_GetRoutes;
  }
  else if(methodStr == "post")
  {
    routeContainer = &m_PostRoutes;
  }
  else if(methodStr == "put")
  {
    routeContainer = &m_PutRoutes;
  }
  else if(methodStr == "delete")
  {
    routeContainer = &m_DeleteRoutes;
  }

  if(routeContainer == nullptr)
  {
    LOG_WARN("Invalid http "
             "method [" << method << "] "
             "action [" << actionName << "] "
             "route [" << route << "]");

    return false;
  }

  LOG_DEBUG("method [" << method << "] "
            "action [" << actionName << "] "
            "route [" << route << "]");

  bool containsKey = (routeContainer->find(route) != routeContainer->end());

  // Initialize and assign the Route struct.
  (*routeContainer)[route] = Route(route, actionName);

  return !containsKey;
}

bool HttpServer::addProcessor(std::shared_ptr<Processor>& processor)
{
  if(processor.get() == nullptr)
  {
    return false;
  }
  m_Processors.push_back(processor);
  return true;
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

