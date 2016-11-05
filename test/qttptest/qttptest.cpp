#include <qttptest.h>

using namespace std;
using namespace qttp;
using namespace qttp::test;
using namespace native::http;

class QttpTest : public QObject, public TestUtils
{
  Q_OBJECT

  private slots:

    void initTestCase();

    void testGET_EchoResponse();
    void testGET_NoEchoResponse();
    void testPOST_EchoBodyResponse();
    void testPOST_InvalidEchoBodyResponse();

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

    void testPOST_InvalidTerminateResponse();

    void testGET_TerminateResponse();

    void testPUT_TerminateResponse();

    void testDELETE();

    void testDEL();

    void cleanupTestCase();
};

void QttpTest::testGET_EchoResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"C++ FTW 123\",\"postprocess\":true}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/echo/123/data", expected);
}

void QttpTest::testGET_NoEchoResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/echo/123", expected);
}

void QttpTest::testPOST_EchoBodyResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":{\"hi\":\"there\"},\"postprocess\":true}";
  QByteArray result;
  QByteArray body = "{\"hi\":\"there\"}";
  TestUtils::requestValidPost("http://127.0.0.1:8080/echobody",
                              result,
                              body);
  TestUtils::verifyJson(result, expected);
}

void QttpTest::testPOST_InvalidEchoBodyResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":{},\"postprocess\":true}";
  QByteArray result;
  QByteArray body = "should be invalid json";
  TestUtils::requestValidPost("http://127.0.0.1:8080/echobody",
                              result,
                              body);
  TestUtils::verifyJson(result, expected);
}

void QttpTest::testGET_DefaultResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080", expected);
}

void QttpTest::testGET_RandomLocalhostUrl()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/wjlekwjfklje", expected);
}

void QttpTest::testPOST_RandomLocalhostUrl()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}";
  QByteArray result;
  TestUtils::requestValidPost("http://127.0.0.1:8080/wjlekwjfklje",
                              result);
  TestUtils::verifyJson(result, expected);
}

void QttpTest::testGET_TestResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"Test C++ FTW\"}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/test", expected);
}

void QttpTest::testGET_TestConfigResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"Test C++ FTW\"}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/test3", expected);
}

void QttpTest::testGET_SampleResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"Sample C++ FTW\",\"postprocess\":true}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/sample", expected);
}

void QttpTest::testGET_SampleWithParamResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"Sample C++ FTW With Parameter param\",\"postprocess\":true}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/sampleWithParameter", expected);
}

void QttpTest::testGET_SampleWithHttpMethodsResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"Sample C++ FTW Get\",\"postprocess\":true}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/http", expected);
}

void QttpTest::testPOST_SampleWithHttpMethodsResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"Sample C++ FTW Post\",\"postprocess\":true}";
  QByteArray result;
  TestUtils::requestValidPost("http://127.0.0.1:8080/http",
                              result);
  TestUtils::verifyJson(result, expected);
}

void QttpTest::testPUT_SampleWithHttpMethodsResponse()
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

void QttpTest::testDELETE_SampleWithHttpMethodsResponse()
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

void QttpTest::testGET_TerminateResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"Test C++ FTW\"}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/terminates", expected);
}

void QttpTest::testPOST_InvalidTerminateResponse()
{
  QByteArray expected = "{\"preprocess\":true,\"response\":\"C++ FTW\",\"postprocess\":true}";
  QByteArray result;
  TestUtils::requestValidPost("http://127.0.0.1:8080/terminates2",
                              result);
  TestUtils::verifyJson(result, expected);
}

void QttpTest::testPUT_TerminateResponse()
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

void QttpTest::testDELETE()
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

void QttpTest::testDEL()
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

// *****************************************************************//
// *************************** END TESTS ***************************//
// *****************************************************************//

void QttpTest::initTestCase()
{
  HttpServer* httpSvr = TestUtils::setUp(this);

  QVERIFY(httpSvr != nullptr);
  QVERIFY(httpSvr->initialize() == true);

  auto action = httpSvr->createAction("", [](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "C++ FTW";
  });

  httpSvr->addProcessor<SampleProcessor>();

  action = httpSvr->createAction("echo", [](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    auto& query = data.getRequest().getQuery();
    json["response"] = "C++ FTW " + query.queryItemValue("id");
  });

  auto result = httpSvr->registerRoute("get", "", "/echo/:id");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("get", "echo", "/echo/:id/data");
  QVERIFY(result == true);

  action = httpSvr->createAction("echobody", [](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = data.getRequest().getJson();
  });

  result = httpSvr->registerRoute("post", "echobody", "/echobody");
  QVERIFY(result == true);

  // Uses the action interface.
  QVERIFY(httpSvr->addAction<SampleActionWithHttpMethods>().get() != nullptr);

  result = httpSvr->registerRoute("get", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("post", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("put", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("delete", "sampleWithHttpMethods", "/http");
  QVERIFY(result == true);

  // Uses the action interface.
  QString param = "param";
  QVERIFY((httpSvr->addAction<ActionWithParameter, QString>(param)).get() != nullptr);

  result = httpSvr->registerRoute("get", "sampleWithParameter", "/sampleWithParameter");
  QVERIFY(result == true);

  // Uses the action interface.
  QVERIFY((httpSvr->addAction<SampleAction>()).get() != nullptr);

  result = httpSvr->registerRoute("get", "sample", "/sample");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("get", "sample", "/sample2");
  QVERIFY(result == true);

  // Uses a raw std::function based callback.
  action = httpSvr->createAction("test", [](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "Test C++ FTW";

    // NOTE: This terminates early so we should not expect any post-processing.
    data.getResponse().finish();
  });
  QVERIFY((action.get() != nullptr));

  result = httpSvr->registerRoute("get", "test", "/test");
  QVERIFY(result == true);

  result = httpSvr->registerRoute("get", "test", "/test2");
  QVERIFY(result == true);

  action = httpSvr->createAction("terminates", [](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "Test C++ FTW";
    // NOTE: This terminates early so we should not expect any post-processing.
    data.getResponse().terminate();
  });
  QVERIFY((action.get() != nullptr));

  result = httpSvr->registerRoute("get", "terminates", "/terminates");
  QVERIFY(result == true);

  httpSvr->startServer("127.0.0.1", 8080);
  QTest::qWait(1000);
}

void QttpTest::cleanupTestCase()
{
  TestUtils::tearDown();
}

QTEST_MAIN(QttpTest)
#include "qttptest.moc"
