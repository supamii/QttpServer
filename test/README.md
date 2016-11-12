# QttpServer Tests

## Quick Start

The QttpServer framework can help you test your API.  Along with using QtTest,
additional hooks and MACROs are also provided to further increase testability.

## Writing Tests

With QtTest, we can write our unit tests like such:

``` c++
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
    void test();
    void cleanupTestCase();
};
```

Above, we extend from `QObject` as well as `TestUtils`. For API testing, it's typical to make a 
series of http GET/PUT/POST calls and we've rolled that up into `TestUtils` for convenience. Then 
below we'll define the main `tests()` method that utilizes `TestUtils`:

``` c++
void QttpTest::test()
{
  QByteArray expected = "{\"hello\":\"world\"}";
  TestUtils::verifyGetJson("http://127.0.0.1:8080/test", expected);
}
```

And below we'll go wrap it up and include setting up the server configurations:

``` c++
void QttpTest::initTestCase()
{
  HttpServer* httpSvr = TestUtils::setUp(this);
  QVERIFY(httpSvr != nullptr);
  QVERIFY(httpSvr->initialize() == true);

  auto action = httpSvr->createAction("", [](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "C++ FTW";
  });
  action->registerRoute(qttp::HttpMethod::GET, "/test");

  httpSvr->startServer("127.0.0.1", 8080);
  QTest::qWait(1000);
}

void QttpTest::cleanupTestCase()
{
  TestUtils::tearDown();
}

QTEST_MAIN(QttpTest)
#include "qttptest.moc"
```

## Test Output

Take a peek at [QttpTest](./qttptest/):

```
Starting qttptest...
********* Start testing of QttpTest *********
Config: Using QtTest library 5.7.0
PASS   : QttpTest::initTestCase()
PASS   : QttpTest::testGET_EchoResponse()
PASS   : QttpTest::testGET_NoEchoResponse()
PASS   : QttpTest::testPOST_EchoBodyResponse()
PASS   : QttpTest::testPOST_InvalidEchoBodyResponse()
PASS   : QttpTest::testGET_DefaultResponse()
PASS   : QttpTest::testGET_RandomLocalhostUrl()
PASS   : QttpTest::testPOST_RandomLocalhostUrl()
PASS   : QttpTest::testGET_TestResponse()
PASS   : QttpTest::testGET_TestConfigResponse()
PASS   : QttpTest::testGET_SampleResponse()
PASS   : QttpTest::testGET_SampleWithParamResponse()
PASS   : QttpTest::testGET_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testPOST_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testPUT_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testDELETE_SampleWithHttpMethodsResponse()
PASS   : QttpTest::testPOST_InvalidTerminateResponse()
PASS   : QttpTest::testGET_TerminateResponse()
PASS   : QttpTest::testPUT_TerminateResponse()
PASS   : QttpTest::testDELETE()
PASS   : QttpTest::testDEL()
PASS   : QttpTest::cleanupTestCase()
Totals: 22 passed, 0 failed, 0 skipped, 0 blacklisted, 7565ms
********* Finished testing of QttpTest *********
qttptest exited with code 0
```

## Monkey Tests

With QttpServer, we explore registered URL paths and routes to produce [Monkey-Tests](./monkeytest/) 
that can provide a *certain* level of confidence before running unit and integration 
tests.