#include <httpserver.h>
#include <swagger.h>

using namespace std;
using namespace qttp;
using namespace native::http;

class Simple : public Action
{
  public:
    Simple() : Action()
    {
    }

    const char* getName() const
    {
      return "simple";
    }

    const QStringList& getTags() const
    {
      static const QStringList list = { "tag1", "tag2" };
      return list;
    }

    const QList<Input>& getInputs() const
    {
      static const QList<Input> list =
      {
        Input("someinput"),
        RequiredInput("reqinput"),
        Input("options", { "selectone", "selectanother" })
      };
      return list;
    }

    void onGet(HttpData& data)
    {
      data.getResponse().getJson()["data"] = "GET ok";
    }

    void onPost(HttpData& data)
    {
      data.getResponse().getJson()["data"] = "POST ok";
    }
};

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
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = "Hello World!";
    });

    svr->registerRoute("post", "echobody", "/echobody");
    svr->addAction("echobody", [](HttpData& data) {
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = data.getRequest().getJson();
    });

    svr->addAction<Simple>();
    svr->registerRoute(HttpMethod::GET, "simple", "/simple");
    svr->registerRoute(HttpMethod::POST, "simple", "/simple");

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
