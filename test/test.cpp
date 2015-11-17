#include <native.h>
#include <QCoreApplication>
#include <QtCore>
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
    void toUpper();
};

void TestHttpServer::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(TestHttpServer)
#include "test.moc"
