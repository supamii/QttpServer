#include "testHttpServer.h"

using namespace std;
using namespace qttp;
using namespace native::http;

class TestHttpServer : public QObject
{
  Q_OBJECT

  private slots:
    void initTestCase();

    void testGET_EchoResponse();
    void testGET_NoEchoResponse();
    void testGET_EchoBodyResponse();
    void testGET_InvalidEchoBodyResponse();

    void testGET_DefaultResponse();
    void testGET_RandomLocalhostUrl();
    void testPOST_RandomLocalhostUrl();

    void testGET_TestResponse();
    void testGET_TestConfigResponse();

    void testGET_SampleResponse();

    void testGET_SampleWithParamResponse();

    void testGET_SampleWithHttpMethodsResponse();
    void testPOST_SampleWithHttpMethodsResponse();
    void testPUT_SampleWithHttpMethodsResponse();
    void testDELETE_SampleWithHttpMethodsResponse();

    void testGET_TerminateResponse();
    void testPOST_InvalidTerminateResponse();
    void testPUT_TerminateResponse();

    void testDELETE();
    void testDEL();
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

  result = httpSvr.addAction("echo", [](HttpData& data) {
    QJsonObject& json = data.getJson();
    json["response"] = "C++ FTW " + data.getQuery().queryItemValue("id");
  });

  result = httpSvr.registerRoute("get", "", "/echo/:id");
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "echo", "/echo/:id/data");
  QVERIFY(result == true);

  result = httpSvr.addAction("echobody", [](HttpData& data) {
    QJsonObject& json = data.getJson();
    json["response"] = data.getRequestParams();
  });

  result = httpSvr.registerRoute("post", "echobody", "/echobody");
  QVERIFY(result == true);

  // Uses the action interface.
  result = httpSvr.addAction<SampleActionWithHttpMethods>();
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  result = httpSvr.registerRoute("post", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  result = httpSvr.registerRoute("put", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  result = httpSvr.registerRoute("delete", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  // Uses the action interface.
  QString param = "param";
  result = httpSvr.addAction<ActionWithParameter, QString>(param);
  QVERIFY(result == true);

  result = httpSvr.registerRoute("get", "sampleWithParameter", "/sampleWithParameter");
  QVERIFY(result == true);

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

void TestHttpServer::testGET_EchoResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/echo/123/data")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"C++ FTW 123\",\"postprocess\":true}").toLatin1());

  string expectedStr = expected.toJson().trimmed().toStdString();
  QVERIFY2(result.toStdString() == expectedStr,
           result.toStdString().c_str());
}

void TestHttpServer::testGET_NoEchoResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/echo/123")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testGET_EchoBodyResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  QNetworkRequest networkRequest(QUrl("http://127.0.0.1:8080/echobody"));
  networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
  QString str = "{\"hi\":\"there\"}";
  netMgr->post(networkRequest, str.toLatin1());

  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":{\"hi\":\"there\"},\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testGET_InvalidEchoBodyResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  QNetworkRequest networkRequest(QUrl("http://127.0.0.1:8080/echobody"));
  networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
  QString str = "should be invalid json";
  netMgr->post(networkRequest, str.toLatin1());

  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":{},\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
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

void TestHttpServer::testGET_SampleWithParamResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/sampleWithParameter")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Sample C++ FTW With Parameter param\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testGET_SampleWithHttpMethodsResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->get(QNetworkRequest(QUrl("http://127.0.0.1:8080/http")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Sample C++ FTW Get\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testPOST_SampleWithHttpMethodsResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  QNetworkRequest networkRequest(QUrl("http://127.0.0.1:8080/http"));
  networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
  netMgr->post(networkRequest, QByteArray());
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Sample C++ FTW Post\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testPUT_SampleWithHttpMethodsResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  QNetworkRequest networkRequest(QUrl("http://127.0.0.1:8080/http"));
  networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
  netMgr->put(networkRequest, QByteArray());
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Sample C++ FTW Put\",\"postprocess\":true}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testDELETE_SampleWithHttpMethodsResponse()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  QNetworkRequest networkRequest(QUrl("http://127.0.0.1:8080/http"));
  networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
  netMgr->deleteResource(networkRequest);
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"preprocess\":true,\"response\":\"Sample C++ FTW Delete\",\"postprocess\":true}").toLatin1());
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

void TestHttpServer::testDELETE()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->deleteResource(QNetworkRequest(QUrl("http://127.0.0.1:8080/testDelete")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"postprocess\":true,\"preprocess\":true,\"response\":\"Sample C++ FTW Delete\"}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

void TestHttpServer::testDEL()
{
  QString result;
  QNetworkAccessManager* netMgr = new QNetworkAccessManager();
  QObject::connect(netMgr, &QNetworkAccessManager::finished, [&result](QNetworkReply* reply)
  {
    result = QString(reply->readAll()).trimmed();
  });
  netMgr->deleteResource(QNetworkRequest(QUrl("http://127.0.0.1:8080/testDel")));
  QVERIFY(result.isEmpty());
  QTest::qWait(1000);
  QJsonDocument expected;
  expected = QJsonDocument::fromJson(QString("{\"postprocess\":true,\"preprocess\":true,\"response\":\"Sample C++ FTW Delete\"}").toLatin1());
  QVERIFY2(result.toStdString() == expected.toJson().trimmed().toStdString(),
           result.toStdString().c_str());
}

QTEST_MAIN(TestHttpServer)
#include "testHttpServer.moc"
