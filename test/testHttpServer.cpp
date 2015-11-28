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

  QVERIFY(httpSvr != nullptr);

  // Uses the action interface.
  bool result = httpSvr->addAction<Sample>();
  QVERIFY(result == true);

  result = httpSvr->registerRoute("/sample", "sample");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("/sample2", "sample");
  QVERIFY(result == true);

  // Uses a raw std::function based callback.
  result = httpSvr->addAction("test", [](HttpData& data) {
    QJsonObject& json = data.getJson();
    json["response"] = "Test C++ FTW";
    data.completeJsonResponse();
  });
  QVERIFY(result == true);

  result = httpSvr->registerRoute("/test", "test");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("/test2", "test");
  QVERIFY(result == true);

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
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"response\":\"C++ FTW\"}").toLatin1());
  QVERIFY(result.toStdString() == expected.toJson().trimmed().toStdString());
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
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"response\":\"C++ FTW\"}").toLatin1());
  QVERIFY(result.toStdString() == expected.toJson().trimmed().toStdString());
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
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"response\":\"C++ FTW\"}").toLatin1());
  QVERIFY(result.toStdString() == expected.toJson().trimmed().toStdString());
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
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"response\":\"Test C++ FTW\"}").toLatin1());
  QVERIFY(result.toStdString() == expected.toJson().trimmed().toStdString());
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
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"response\":\"Sample C++ FTW\"}").toLatin1());
  QVERIFY(result.toStdString() == expected.toJson().trimmed().toStdString());
}

QTEST_MAIN(TestHttpServer)
#include "testHttpServer.moc"
