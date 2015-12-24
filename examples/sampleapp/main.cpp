#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <thread>
#include <iostream>
#include <stdexcept>

#include <httpserver.h>

using namespace std;
using namespace qttp;
using namespace native::http;

int main(int argc, char** argv)
{
  auto result = -1;

  try
  {
    LOG_TRACE;

    QCoreApplication app(argc, argv);

    HttpServer* svr = HttpServer::getInstance();

    // Always initialize in the main thread.
    svr->initialize(&app);

    svr->registerRoute("get", "helloworld", "/helloworld");

    svr->addAction("helloworld", [](HttpData& data) {
      QJsonObject& json = data.getJson();
      json["response"] = "Hello World!";
    });

    thread webSvr(HttpServer::start);
    webSvr.detach();

    result = app.exec();
  }
  catch(std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "Caught and unknown exception" << std::endl;
  }

  // TODO: Shutdown the webserver!
  return result;
}
