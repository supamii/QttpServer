#include <testutils.h>

using namespace std;
using namespace qttp;
using namespace qttp::test;
using namespace native::http;

class MonkeyTest : public QObject, public TestUtils
{
  Q_OBJECT

  private slots:

    void initTestCase();

    void test();

    void cleanupTestCase();
};

void MonkeyTest::test()
{
  HttpServer* httpSvr = HttpServer::getInstance();
  const char* baseUrl = "http://127.0.0.1:8080";

  for(int i = 0; i < (int) httpSvr->m_Routes.size(); ++i)
  {
    auto & routes = httpSvr->m_Routes.at(i);
    for(Route& route : routes)
    {
      switch(i)
      {
        case HttpMethod::GET:
          requestGet(baseUrl + route.path);
          requestGet(baseUrl + route.path.replace(":", "replace"));
          requestGet(baseUrl + route.path.append("?test=value"));
          requestGet(baseUrl + route.path.replace(":", "replace").append("?test=value"));
          break;
        case HttpMethod::POST:
          requestPost(baseUrl + route.path);
          requestPost(baseUrl + route.path.replace(":", "replace"));
          requestPost(baseUrl + route.path.append("?test=value"));
          requestPost(baseUrl + route.path.replace(":", "replace").append("?test=value"));
          break;
        default:
          break;
      }
    }
  }
}

void MonkeyTest::initTestCase()
{
  HttpServer* httpSvr = TestUtils::setUp(this);

  QVERIFY(httpSvr != nullptr);
  QVERIFY(httpSvr->initialize() == true);

  auto action = httpSvr->createAction([](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "C++ FTW";
  });
  action->registerRoute(HttpMethod::GET, "something");

  httpSvr->startServer("127.0.0.1", 8080);
  QTest::qWait(1000);
}

void MonkeyTest::cleanupTestCase()
{
  TestUtils::tearDown();
}

QTEST_MAIN(MonkeyTest)
#include "monkeytest.moc"
