#include <httpserver.h>

using namespace std;
using namespace qttp;
using namespace native::http;

int main(int argc, char** argv)
{
  auto result = 0;

  try
  {
    LOG_TRACE;

    QCoreApplication app(argc, argv);

    HttpServer* svr = HttpServer::getInstance();

    // Always initialize in the main thread.
    svr->initialize();

    svr->registerRoute("get", "helloworld", "/helloworld");
    svr->addAction("helloworld", [](HttpData& data) {
      QJsonObject& json = data.getJson();
      json["response"] = "Hello World!";

      //json["response"] = data.getHttpRequest().getBody() +
      //                   data.getHttpRequest().getUrl().getPath();
    });

    svr->registerRoute("post", "echobody", "/echobody");
    svr->addAction("echobody", [](HttpData& data) {
        QJsonObject& json = data.getJson();
        json["response"] = data.getRequestParams();
    });

    svr->startServer();

    result = app.exec();
  }
  catch(std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  catch(...)
  {
    std::cerr << "ERROR: Caught an unknown exception" << std::endl;
  }

  return result;
}
