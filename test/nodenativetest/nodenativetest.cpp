#include "nodenativetest.h"

using namespace std;
using namespace native::http;

class NodeNativeTest : public QObject, public qttp::test::TestUtils
{
  Q_OBJECT

  private slots:

    void requestGet();
    void requestGetBang();
    void requestPost();
    void requestPut();
    void requestDelete();

    void initTestCase();
    void cleanupTestCase();
};

void NodeNativeTest::requestGet()
{
  QByteArray result;
  qttp::test::TestUtils::requestGet("http://127.0.0.1:8080", result);
  QCOMPARE(result.constData(), "Hello World GET");
}

void NodeNativeTest::requestGetBang()
{
  QByteArray result;
  qttp::test::TestUtils::requestGet("http://127.0.0.1:8080/bang", result);
  QCOMPARE(result.constData(), "Hello World! GET");
}

void NodeNativeTest::requestPost()
{
  QByteArray result;
  qttp::test::TestUtils::requestPost("http://127.0.0.1:8080", result);
  QCOMPARE(result.constData(), "Hello World POST");
}

void NodeNativeTest::requestPut()
{
  QByteArray result;
  qttp::test::TestUtils::requestPut("http://127.0.0.1:8080", result);
  QCOMPARE(result.constData(), "Hello World PUT");
}

void NodeNativeTest::requestDelete()
{
  QByteArray result;
  qttp::test::TestUtils::requestDelete("http://127.0.0.1:8080", result);
  QCOMPARE(result.constData(), "Hello World DELETE");
}

int startServer()
{
  http httpServer;
  httpServer.listen("127.0.0.1", 8080, [](request& req, response& resp) {
    string result = "Hello World";
    if(req.url().path() == "/bang")
    {
      result += "!";
    }
    result += " " + req.get_method();
    resp.write(result);
    resp.close();
  });
  return native::run();
}

void NodeNativeTest::initTestCase()
{
  std::thread newThread(startServer);
  newThread.detach();
}

void NodeNativeTest::cleanupTestCase()
{
}

QTEST_MAIN(NodeNativeTest)
#include "nodenativetest.moc"
