#include <httpserver.h>
#include "sampleAction.h"
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

    void testGET_TestResponse();
    void testGET_TestConfigResponse();

    void testGET_SampleResponse();

    void testGET_TerminateResponse();
    void testPOST_InvalidTerminateResponse();
    void testPUT_TerminateResponse();
};

void TestHttpServer::initTestCase()
{
  HttpServer& httpSvr = *(HttpServer::getInstance());
  httpSvr.initialize();

  QVERIFY(HttpServer::getInstance() != nullptr);

  bool result = httpSvr.addAction("", [](HttpData& data) {
      QJsonObject& json = data.getJson();
      json["response"] = "C++ FTW";
  });

  httpSvr.addProcessor<SampleProcessor>();

  // Uses the action interface.
  result = httpSvr.addAction<SampleAction>();
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "sample", "/sample");
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "sample", "/sample2");
  QVERIFY(result == true);

  // Uses a raw std::function based callback.
  result = httpSvr.addAction("test", [](HttpData& data) {
    QJsonObject& json = data.getJson();
    json["response"] = "Test C++ FTW";

    // NOTE: This terminates early so we should not expect any post-processing.
    data.finishResponse();
  });
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "test", "/test");
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "test", "/test2");
  QVERIFY(result == true);

  result = httpSvr.addAction("terminates", [](HttpData& data) {
    QJsonObject& json = data.getJson();
    json["response"] = "Test C++ FTW";
    // NOTE: This terminates early so we should not expect any post-processing.
    data.setTerminated();
  });
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "terminates", "/terminates");
  QVERIFY(result == true);

  httpSvr.startServer();
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
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
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
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
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
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
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
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Test C++ FTW\"}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testGET_TestConfigResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/test3")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Test C++ FTW\"}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
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
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Sample C++ FTW\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testGET_TerminateResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/terminates")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Test C++ FTW\"}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testPOST_InvalidTerminateResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->post(QNetworkRequest(QUrl("http://127.0.0.1:8080/terminates2")), QByteArray());
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testPUT_TerminateResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->put(QNetworkRequest(QUrl("http://127.0.0.1:8080/terminates2")), QByteArray());
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Test C++ FTW\"}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

QTEST_MAIN(TestHttpServer)
#include "testHttpServer.moc"
