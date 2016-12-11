#include <qttpserver>
#include <swagger.h>

using namespace std;
using namespace qttp;
using namespace native::http;

class Another : public Action
{
  public:
    Another() : Action()
    {
    }

    const char* getName() const
    {
      return "another";
    }

    QStringList getTags() const
    {
      return { "tag1", "tag2" };
    }

    std::vector<Input> getInputs() const
    {
      return {
               Input("someinput"),
               RequiredInput("reqinput"),
               Input("options", "someoptions", { "selectone", "selectanother" })
      };
    }

    std::set<qttp::HttpPath> getRoutes() const
    {
      static const QString route = "/another";
      return {
               { HttpMethod::GET, route },
               { HttpMethod::POST, route }
      };
    }

    void onGet(HttpData& data)
    {
      data.getResponse().getJson()["data"] = QSTR("GET another");
    }

    void onPost(HttpData& data)
    {
      data.getResponse().getJson()["data"] = QSTR("POST another");
    }
};

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

    QStringList getTags() const
    {
      static const QStringList list = { "tag1", "tag2" };
      return list;
    }

    std::vector<Input> getInputs() const
    {
      static const std::vector<Input> list =
      {
        Input("someinput"),
        RequiredInput("reqinput"),
        Input("options", "some options", { "selectone", "selectanother" })
      };
      return list;
    }

    void onGet(HttpData& data)
    {
      data.getResponse().getJson()["data"] = QSTR("GET ok");
    }

    void onPost(HttpData& data)
    {
      data.getResponse().getJson()["data"] = QSTR("POST ok");
    }

    void onPut(HttpData& data)
    {
      data.getResponse().getJson()["data"] = QSTR("PUT ok");
    }

    void onPatch(HttpData& data)
    {
      data.getResponse().getJson()["data"] = QSTR("PATCH ok");
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
    if(!svr->initialize()) {
      std::cerr << "Failed to initialize!" << std::endl;
    }

    auto helloworld = svr->createAction([](HttpData& data) {
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = QSTR("Hello World!");
    });

    helloworld->registerRoute(HttpMethod::GET, "/helloworld");

    auto echobody = svr->createAction([](HttpData& data) {
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = data.getRequest().getJson();
    });

    echobody->registerRoute({
      { HttpMethod::GET, "echobody" },
      { HttpMethod::GET, "echobody2" },
      { HttpMethod::POST, "echobody3" }
    });

    svr->addActionAndRegister<Simple>("/simple",
                                      { HttpMethod::GET,
                                        HttpMethod::POST });

    svr->addActionAndRegister<Simple>("/simple",
                                      { "PUT", "PATCH" });

    svr->addActionAndRegister<Another>();

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
