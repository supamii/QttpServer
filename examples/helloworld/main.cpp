#include <httpserver.h>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  // Always initialize in the main thread.
  qttp::HttpServer* httpSvr = qttp::HttpServer::getInstance();
  httpSvr->initialize();

  // Create an action that will handle all requests
  auto action = httpSvr->createAction([](qttp::HttpData& data) {
    // Form the JSON content and let the framework handle the rest.
    QJsonObject& json = data.getResponse().getJson();
    json["hello"] = "world";
  });

  // Register the action to handle http GET for the path "/".
  action->registerRoute(qttp::HttpMethod::GET, "/");

  // Register the action to handle http GET for the path "/hello".
  action->registerRoute(qttp::HttpMethod::GET, "/hello");

  // Libuv runs in its own thread.
  httpSvr->startServer();

  // Qt takes the main thread per the usual.
  return app.exec();
}
