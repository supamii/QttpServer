#include <qttpserver>

using namespace qttp;

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  HttpServer* svr = HttpServer::getInstance();
  svr->initialize();

  auto helloworld = svr->createAction([](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = "Hello World!";
  });

  helloworld->registerRoute(HttpMethod::GET, "/helloworld");

  auto echobody = svr->createAction([](HttpData& data) {
    QJsonObject& json = data.getResponse().getJson();
    json["response"] = data.getRequest().getJson();
  });

  echobody->registerRoute(HttpMethod::POST, "/echobody");

  svr->startServer();
  return app.exec();
}
