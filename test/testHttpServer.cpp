#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <QtNetwork>
#include <thread>
#include "httpserver.h"

#include <QtTest/QtTest>

using namespace std;
using namespace qttp;
using namespace native::http;

class TestHttpServer: public QObject
{
  Q_OBJECT

  private slots:
    void initTestCase();
    void testGET_DefaultResponse();
    void testGET_RandomLocalhostUrl();
    void testPOST_RandomLocalhostUrl();
};

void TestHttpServer::initTestCase()
{
  Q_ASSERT(HttpServer::getInstance() != nullptr);
  std::thread webSvr(HttpServer::start);
  webSvr.detach();
}

void TestHttpServer::testGET_DefaultResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080")));
  Q_ASSERT(result.isEmpty());
  QTest::qWait(1000);
  Q_ASSERT(result == "C++ FTW");
}

void TestHttpServer::testGET_RandomLocalhostUrl()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/wjlekwjfklje")));
  Q_ASSERT(result.isEmpty());
  QTest::qWait(1000);
  Q_ASSERT(result == "C++ FTW");
}

void TestHttpServer::testPOST_RandomLocalhostUrl()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  QNetworkRequest networkRequest(QUrl("http://127.0.0.1:8080/wjlekwjfklje"));
  networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
  netMgr->post(networkRequest, QByteArray());
  Q_ASSERT(result.isEmpty());
  QTest::qWait(1000);
  Q_ASSERT(result == "C++ FTW");
}

QTEST_MAIN(TestHttpServer)
#include "testHttpServer.moc"
