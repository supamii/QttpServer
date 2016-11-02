#include <httpserver.h>
#include "mainwindow.h"
#include <QApplication>

using namespace std;
using namespace qttp;
using namespace native::http;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  MainWindow w;
  w.show();

  HttpServer* svr = HttpServer::getInstance();
  svr->initialize();
  svr->registerRoute("get", "helloworld", "/helloworld");
  svr->createAction("helloworld", [&](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "Hello World!";

    w.getModel().addData(data);
  });
  svr->startServer();

  return a.exec();
}
