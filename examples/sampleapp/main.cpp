#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <thread>

#include <httpserver.h>

using namespace std;
using namespace qttp;
using namespace native::http;

int main(int argc, char** argv)
{
  LOG_TRACE;

  QCoreApplication app(argc, argv);

  // Always initialize in the main thread.
  HttpServer::getInstance();

  thread webSvr(HttpServer::start);
  webSvr.detach();

  auto result = app.exec();

  // TODO: Shutdown the webserver.
  return result;
}
