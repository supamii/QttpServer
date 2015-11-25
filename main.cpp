#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <thread>
//#include <mongo/client/dbclient.h>

#include "httpserver.h"

// #include "sample.h"

using namespace std;
using namespace qttp;
using namespace native::http;

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  // mongo::client::initialize();

  // Always initialize in the main thread.
  HttpServer* httpSvr = HttpServer::getInstance();

  // Example 1 - Uses the action interface.
  // httpSvr->addAction<Sample>();

  // Example 2 - Uses a raw std::function based callback.
  //httpSvr->addAction("test", [](native::http::request*, native::http::response* resp) {
  //  resp->set_status(200);
  //  resp->set_header("Content-Type", "text/plain");
  //  resp->end("Test C++ FTW\n");
  //});

  // Example 3 - Bind routes and actions together.
  //httpSvr->registerRoute("/test", "test");
  //httpSvr->registerRoute("/test2", "test");

  thread webSvr(HttpServer::start);
  webSvr.detach();

  auto result = app.exec();

  // TODO: Shutdown the webserver.
  return result;
}
