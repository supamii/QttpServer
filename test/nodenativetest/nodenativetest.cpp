#include <testutils.h>

using namespace std;
using namespace qttp;
using namespace qttp::test;
using namespace native::http;

class NodeNativeTest : public QObject, public TestUtils
{
  Q_OBJECT

  private slots:

    void initTestCase();

    void test();

    void cleanupTestCase();
};

void NodeNativeTest::test()
{
}

void NodeNativeTest::initTestCase()
{
}

void NodeNativeTest::cleanupTestCase()
{
}

QTEST_MAIN(NodeNativeTest)
#include "nodenative.moc"
