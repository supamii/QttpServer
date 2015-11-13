#include "httpserver.h"
#include "httpevent.h"

using namespace std;
using namespace native::http;

unique_ptr<HttpServer> HttpServer::m_Instance(nullptr);

HttpServer* HttpServer::getInstance() {
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

  // TODO: Ip interface should be configurable (later).

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
  return [](request* req, response* resp) {
    QString url = QString::fromStdString(req->url().path());
    qDebug() << "http method" << req->get_method().c_str();
    if(url.startsWith("/test"))
    {
      qDebug() << "Sending event";

      resp->set_status(200);
      resp->set_header("Content-Type", "text/plain");

      QNetworkAccessManager* netMgr = new QNetworkAccessManager();
      QObject::connect(netMgr, &QNetworkAccessManager::finished, [req, resp](QNetworkReply* reply)
      {
        const auto body = req->get_body();
        qDebug() << "QNetworkAccessManager::finished" << body.c_str();
        resp->end("C++ FTW\n");
      });
      netMgr->get(QNetworkRequest(QUrl("http://www.qt.io")));

    } else {

      resp->set_status(400);
      resp->set_header("Content-Type", "text/plain");
      resp->end("Not allowed\n");
      qDebug() << "Rejected" << url;
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

// Route mapping scheme
// Pre and Post processors
