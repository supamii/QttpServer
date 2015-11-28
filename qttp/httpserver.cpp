#include "httpevent.h"
#include "httpserver.h"

using namespace std;
using namespace qttp;
using namespace native::http;

unique_ptr<HttpServer> HttpServer::m_Instance(nullptr);

HttpServer* HttpServer::getInstance()
{
  if(m_Instance.get() == nullptr) {
      m_Instance.reset(new HttpServer());
  }
  return m_Instance.get();
}

HttpServer::HttpServer() : QObject(), m_EventCallback(this->defaultEventCallback())
{
  this->installEventFilter(this);
}

HttpServer::~HttpServer()
{
}

void HttpServer::initialize()
{
  m_GlobalConfig = Utils::readJson(QDir("config/global.json").absolutePath());
  m_RoutesConfig = Utils::readJson(QDir("config/routes.json").absolutePath());
}

int HttpServer::start()
{
  http server;

  HttpServer* svr = HttpServer::getInstance();
  QString ip = svr->m_GlobalConfig["bindIp"].isUndefined() ? "0.0.0.0" : svr->m_GlobalConfig["bindIp"].toString();
  auto port = svr->m_GlobalConfig["bindPort"].isUndefined() ? 8080 : svr->m_GlobalConfig["bindPort"].toInt();

  auto result = server.listen(ip.toStdString(), port, [](request& req, response& resp) {
    HttpEvent* event = new HttpEvent(&req, &resp);
    QCoreApplication::postEvent(HttpServer::getInstance(), event);
  });

  if(!result)
  {
    qWarning() << "Unable to bind to" << ip << port;
    return 1;
  }

  qDebug() << "Server running at" << ip << port;
  return native::run();
}

void HttpServer::setEventCallback(function<void(request*, response*)> eventCallback)
{
  m_EventCallback = eventCallback;
}

function<void(request*, response*)> HttpServer::defaultEventCallback() const
{
  // TODO: Pre and Post processors?
  // TODO: Perhaps should lock/wrap m_Routes to guarantee atomicity.

  return [this](request* req, response* resp)
  {
    HttpData data(req, resp);

    auto lookup = m_Routes.find(req->url().path());
    if(lookup != m_Routes.end())
    {
      auto callback = this->m_ActionCallbacks.find(lookup->second);
      if(callback != this->m_ActionCallbacks.end())
      {
        callback->second(data);
        return;
      }

      auto action = this->m_Actions.find(lookup->second);
      if(action != this->m_Actions.end() && action->second.get() != nullptr)
      {
        action->second->onAction(data);
        return;
      }
    }
    else
    {
      QJsonObject& json = data.getJson();
      json["response"] = "C++ FTW";
      if(!data.finishJsonResponse())
      {
        qWarning() << "Failed to finish default response";
      }
    }
  };
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
    qWarning() << "Request or response is NULL";
  }

  m_EventCallback(req, resp);
  return true;
}

bool HttpServer::addAction(const string& actionName, function<void(HttpData& data)> callback)
{
  bool containsKey = (m_ActionCallbacks.find(actionName) != m_ActionCallbacks.end());
  m_ActionCallbacks[actionName] = callback;
  return !containsKey;
}

bool HttpServer::registerRoute(const string& routeName, const string& actionName)
{
  bool containsKey = (m_Routes.find(routeName) != m_Routes.end());
  m_Routes[routeName] = actionName;
  return !containsKey;
}
