#include <QApplication>
#include <httpserver.h>
#include "mainwindow.h"

using namespace qttp;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  MainWindow w;
  w.show();

  HttpServer* svr = HttpServer::getInstance();
  svr->initialize();

  auto action = svr->createAction([&](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "Hello World!";
    // Record the results and display on the GUI!
    w.getModel().addData(data);
  });
  action->registerRoute(HttpMethod::GET, "/helloworld");

  svr->startServer();
  return a.exec();
}
