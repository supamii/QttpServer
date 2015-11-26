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

HttpServer::HttpServer() : QObject(), m_EventCallback(this->defaultCallback())
{
  this->installEventFilter(this);
}

HttpServer::~HttpServer()
{
}

void HttpServer::initialize()
{
  m_GlobalsConfig = Utils::readJson(QDir("config/global.json").absolutePath());
  m_RoutesConfig = Utils::readJson(QDir("config/routes.json").absolutePath());
}

int HttpServer::start()
{
  http server;

  // TODO: Ip interface should be configurable using globals.json

//  auto ip = m_GlobalsConfig["bindIp"].isUndefined() ? "0.0.0.0" : m_GlobalsConfig["bindIp"].toString().toStdString();
//  auto port = m_GlobalsConfig["bindPort"].isUndefined() ? 8080 : m_GlobalsConfig["bindPort"].toInt();

  auto ip = "0.0.0.0";
  auto port = 8080;

  auto result = server.listen(ip, port, [](request& req, response& resp) {
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

function<void(request*, response*)> HttpServer::defaultCallback() const
{
  // TODO: Pre and Post processors?
  // TODO: Perhaps should lock/wrap m_Routes to guarantee atomicity.
  // TODO: We could optimize by reducing a double-lookup on routes to actions.

  return [this](request* req, response* resp)
  {
    auto lookup = m_Routes.find(req->url().path());
    if(lookup != m_Routes.end())
    {
      auto callback = this->m_ActionCallbacks.find(lookup->second);
      if(callback != this->m_ActionCallbacks.end())
      {
        callback->second(req, resp);
        return;
      }

      auto action = this->m_Actions.find(lookup->second);
      if(action != this->m_Actions.end() && action->second.get() != nullptr)
      {
        action->second->onAction(req, resp);
        return;
      }
    }
    else
    {
      resp->set_status(200);
      resp->set_header("Content-Type", "text/plain");
      resp->end("C++ FTW\n");
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

bool HttpServer::addAction(const std::string& actionName, function<void(native::http::request*, native::http::response*)> callback)
{
  bool containsKey = (m_ActionCallbacks.find(actionName) != m_ActionCallbacks.end());
  m_ActionCallbacks[actionName] = callback;
  return !containsKey;
}

bool HttpServer::registerRoute(const std::string& routeName, const std::string& actionName)
{
  bool containsKey = (m_Routes.find(routeName) != m_Routes.end());
  m_Routes[routeName] = actionName;
  return !containsKey;
}
