#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <QtNetwork>
#include <thread>
#include "httpserver.h"

#include <QtTest/QtTest>

#include "sample.h"

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

    void testGET_TestResponse();
    void testGET_SampleResponse();
};

void TestHttpServer::initTestCase()
{
  HttpServer* httpSvr = HttpServer::getInstance();
  httpSvr->initialize();

  Q_ASSERT(httpSvr != nullptr);

  // Uses the action interface.
  bool result = httpSvr->addAction<Sample>();
  Q_ASSERT(result == true);

  result = httpSvr->registerRoute("/sample", "sample");
  Q_ASSERT(result == true);

  result = httpSvr->registerRoute("/sample2", "sample");
  Q_ASSERT(result == true);

  // Uses a raw std::function based callback.
  result = httpSvr->addAction("test", [](HttpData& data) {
    data.getResponse().set_status(200);
    data.getResponse().set_header("Content-Type", "text/plain");
    data.getResponse().end("Test C++ FTW\n");
  });
  Q_ASSERT(result == true);

  result = httpSvr->registerRoute("/test", "test");
  Q_ASSERT(result == true);

  result = httpSvr->registerRoute("/test2", "test");
  Q_ASSERT(result == true);

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
    qDebug() << result;
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080")));
  Q_ASSERT(result.isEmpty());
  QTest::qWait(3000);
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

void TestHttpServer::testGET_TestResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/test")));
  Q_ASSERT(result.isEmpty());
  QTest::qWait(1000);
  Q_ASSERT(result == "Test C++ FTW");
}

void TestHttpServer::testGET_SampleResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/sample")));
  Q_ASSERT(result.isEmpty());
  QTest::qWait(1000);
  Q_ASSERT(result == "Sample C++ FTW");
}

QTEST_MAIN(TestHttpServer)
#include "testHttpServer.moc"
