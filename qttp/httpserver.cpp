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

HttpServer::HttpServer() : QObject(), m_EventCallback(this->defaultEventCallback()), m_Actions(), m_ActionCallbacks(), m_Routes(), m_Processors(), m_GlobalConfig(), m_RoutesConfig()
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
  // TODO: Perhaps should lock/wrap m_Routes to guarantee atomicity.

  return [=](request* req, response* resp)
  {
    HttpData data(req, resp);

    auto lookup = m_Routes.find(req->url().path());
    if(lookup != m_Routes.end())
    {
      auto callback = m_ActionCallbacks.find(lookup->second);
      if(callback != m_ActionCallbacks.end())
      {
        preprocessAction(data);
        if(data.getControlFlag()) callback->second(data);
        if(data.getControlFlag()) postprocessAction(data);
      }
      else
      {
        auto action = m_Actions.find(lookup->second);
        if(action != m_Actions.end() && action->second.get() != nullptr)
        {
          preprocessAction(data);
          if(data.getControlFlag()) action->second->onAction(data);
          if(data.getControlFlag()) postprocessAction(data);
        }
      }
    }
    else
    {
      preprocessAction(data);

      if(data.getControlFlag())
      {
        QJsonObject& json = data.getJson();
        json["response"] = "C++ FTW";

        postprocessAction(data);
      }
    }

    if(!data.getJson().isEmpty() && !data.isFinished() && !data.finishResponse())
    {
      qWarning() << "Failed to finish response";
    }
  };
}

void HttpServer::preprocessAction(HttpData &data) const
{
  for(auto& processor : m_Processors)
  {
    if(processor.get())
    {
      processor->preprocess(data);
    }
  }
}

void HttpServer::postprocessAction(HttpData &data) const
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

bool HttpServer::registerRoute(const string& routeName, const string& actionName)
{
  bool containsKey = (m_Routes.find(routeName) != m_Routes.end());
  m_Routes[routeName] = actionName;
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
