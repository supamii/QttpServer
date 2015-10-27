#include "httpeventhandler.h"
#include "httpevent.h"

using namespace std;
using namespace native::http;

unique_ptr<HttpEventHandler> HttpEventHandler::m_Instance(nullptr);

HttpEventHandler* HttpEventHandler::getInstance() {
    if(m_Instance.get() == nullptr) {
        m_Instance.reset(new HttpEventHandler());
    }
    return m_Instance.get();
}

HttpEventHandler::HttpEventHandler() : QObject()
{
    this->installEventFilter(this);
}

HttpEventHandler::~HttpEventHandler()
{
}

bool HttpEventHandler::eventFilter(QObject* /* object */, QEvent* event)
{
    if (!event || event->type() != QEvent::None)
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

    // const auto body = req->get_body();
    resp->set_status(200);
    resp->set_header("Content-Type", "text/plain");

    QNetworkAccessManager* netMgr = new QNetworkAccessManager();
    QObject::connect(netMgr, &QNetworkAccessManager::finished, [req, resp](QNetworkReply* reply)
    {
        qDebug() << "QNetworkAccessManager::finished" << reply->readAll();
        resp->end("C++ FTW\n");
    });
    netMgr->get(QNetworkRequest(QUrl("http://www.qt.io")));

    return true;
}

