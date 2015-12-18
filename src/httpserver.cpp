#include "httpserver.h"

#include "httpevent.h"

using namespace std;
using namespace qttp;
using namespace native::http;

unique_ptr<HttpServer> HttpServer::m_Instance(nullptr);

HttpServer* HttpServer::getInstance()
{
  if(m_Instance.get() == nullptr)
  {
      m_Instance.reset(new HttpServer());
  }
  return m_Instance.get();
}

HttpServer::HttpServer() :
    QObject(),
    m_EventCallback(this->defaultEventCallback()),
    m_Actions(),
    m_ActionCallbacks(),
    m_GetRoutes(),
    m_PostRoutes(),
    m_PutRoutes(),
    m_DelRoutes(),
    m_Processors(),
    m_Preprocessors(),
    m_Postprocessors(),
    m_GlobalConfig(),
    m_RoutesConfig()
{
  this->installEventFilter(this);
  this->initialize();
}

HttpServer::~HttpServer()
{
}

void HttpServer::initialize()
{
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
  m_RoutesConfig = Utils::readJson(QDir("config/routes.json").absolutePath());

  QJsonValueRef get = m_RoutesConfig["get"];
  registerRouteFromJSON(get, "get");

  QJsonValueRef post = m_RoutesConfig["post"];
  registerRouteFromJSON(post, "post");

  QJsonValueRef put = m_RoutesConfig["put"];
  registerRouteFromJSON(put, "put");

  QJsonValueRef del = m_RoutesConfig["del"];
  registerRouteFromJSON(del, "del");
}

void HttpServer::registerRouteFromJSON(QJsonValueRef& obj, const std::string& method)
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
        auto action = route["action"].toString().trimmed().toStdString();
        auto path = route["path"].toString().trimmed().toStdString();
        if(route["isActive"] != false && !path.empty())
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
  http server;

  HttpServer* svr = HttpServer::getInstance();
  QString ip = svr->m_GlobalConfig["bindIp"].isUndefined() ? "0.0.0.0" : svr->m_GlobalConfig["bindIp"].toString().trimmed();
  if(ip.isEmpty())
  {
    ip = "0.0.0.0";
    LOG_WARN("Bind ip is invalid, defaulting to" << ip);
  }
  auto port = svr->m_GlobalConfig["bindPort"].isUndefined() ? 8080 : svr->m_GlobalConfig["bindPort"].toInt();
  if(port <= 0)
  {
    port = 8080;
    LOG_WARN("Bind port is invalid, defaulting to" << port);
  }
  auto result = server.listen(ip.toStdString(), port, [](request& req, response& resp) {
    HttpEvent* event = new HttpEvent(&req, &resp);
    QCoreApplication::postEvent(HttpServer::getInstance(), event);
  });

  if(!result)
  {
    LOG_WARN("Unable to bind to" << ip << port);
    return 1;
  }

  LOG_DEBUG("Server running at" << ip << port);
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

  return [=](request* req, response* resp)
  {
    HttpData data(req, resp);

    const unordered_map<string, string>* routes = &m_GetRoutes;
    QString method = QString(req->get_method().c_str()).toLower().trimmed();

    if(method == "get")
    {
      routes = &m_GetRoutes;
    }
    else if(method == "post")
    {
      routes = &m_PostRoutes;
    }
    else if(method == "put")
    {
      routes = &m_PutRoutes;
    }
    else if(method == "delete")
    {
      routes = &m_DelRoutes;
    }

    auto lookup = routes->find(req->url().path());
    if(lookup != routes->end())
    {
      auto callback = m_ActionCallbacks.find(lookup->second);
      if(callback != m_ActionCallbacks.end())
      {
        performPreprocessing(data);
        if(data.getControlFlag()) callback->second(data);
        if(data.getControlFlag()) performPostprocessing(data);
      }
      else
      {
        auto action = m_Actions.find(lookup->second);
        if(action != m_Actions.end() && action->second.get() != nullptr)
        {
          performPreprocessing(data);
          if(data.getControlFlag()) action->second->onAction(data);
          if(data.getControlFlag()) performPostprocessing(data);
        }
      }
    }
    else
    {
      // Even the the action is not yet found, we'll give the user a chance to
      // intercept and process it.
      performPreprocessing(data);

      if(data.getControlFlag())
      {
        // TODO: Describe this in the header file.

        // TODO: Can also perform this check once in a while instead to reduce
        // performance lookup costs.

        // Actions registered under "" is the default handler.
        auto callback = m_ActionCallbacks.find("");
        if(callback != m_ActionCallbacks.end())
        {
          callback->second(data);
          performPostprocessing(data);
        }
        else
        {
          auto action = m_Actions.find("");
          if(action != m_Actions.end())
          {
            action->second->onAction(data);
            performPostprocessing(data);
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

void HttpServer::performPreprocessing(HttpData& data) const
{
  for(auto& callback : m_Preprocessors)
  {
    callback(data);
  }

  for(auto& processor : m_Processors)
  {
    if(processor.get())
    {
      processor->preprocess(data);
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
    }
    ++processor;
  }

  for(auto& callback : m_Postprocessors)
  {
    callback(data);
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

bool HttpServer::addAction(const string& actionName, function<void(HttpData& data)> callback)
{
  bool containsKey = (m_ActionCallbacks.find(actionName) != m_ActionCallbacks.end());
  m_ActionCallbacks[actionName] = callback;
  return !containsKey;
}

bool HttpServer::registerRoute(const std::string& method, const string& actionName, const string& routeName)
{
  unordered_map<string, string>* routeContainer = nullptr;
  QString methodStr = QString::fromStdString(method).trimmed().toLower();
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
    routeContainer = &m_DelRoutes;
  }

  if(routeContainer == nullptr)
  {
    LOG_WARN("Invalid http "
             "method [" << method.c_str() << "] "
             "action [" << actionName.c_str() << "] "
             "route [" << routeName.c_str() << "]");

    return false;
  }

  LOG_DEBUG("method [" << method.c_str() << "] "
            "action [" << actionName.c_str() << "] "
            "route [" << routeName.c_str() << "]");

  bool containsKey = (routeContainer->find(routeName) != routeContainer->end());
  (*routeContainer)[routeName] = actionName;

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
