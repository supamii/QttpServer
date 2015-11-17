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

int HttpServer::start()
{
  http server;

  // TODO: Ip interface should be configurable using globals.json

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
  // TODO:
  // Route mapping scheme
  // Pre and Post processors

  return [](request* /* req */, response* resp) {

    resp->set_status(200);
    resp->set_header("Content-Type", "text/plain");
    resp->end("C++ FTW\n");
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
