#include "sample.h"

Sample::Sample()
{
//  return [](request* /* req */, response* resp) {

//    resp->set_status(200);
//    resp->set_header("Content-Type", "text/plain");
//    resp->end("C++ FTW\n");

    /*
    qDebug() << "http method" << req->get_method().c_str();
    QString url = QString::fromStdString(req->url().path());
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
    /**/
//  };
}

Sample::~Sample()
{
}

