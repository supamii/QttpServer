#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <thread>

#include "httpserver.h"

using namespace std;
using namespace native::http;

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  // Make sure we initialize in the main thread.
  HttpServer::getInstance();

  thread webSvr(HttpServer::start);
  webSvr.detach();

  return app.exec();
}
