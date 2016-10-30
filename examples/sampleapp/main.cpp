#include <httpserver.h>
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

    const QList<std::pair<HttpMethod, QString> >& getRoutes() const
    {
      static const QString route = "/another";
      static const QList<std::pair<HttpMethod, QString> > routes =
      {
        { HttpMethod::GET, route },
        { HttpMethod::POST, route }
      };
      return routes;
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

    // Always initialize in the main thread.
    if(!svr->initialize()) {
      std::cerr << "Failed to initialize!" << std::endl;
    }

    svr->registerRoute("get", "helloworld", "/helloworld");
    svr->addAction("helloworld", [](HttpData& data) {
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = QSTR("Hello World!");
    });

    svr->registerRoute("post", "echobody", "/echobody");
    svr->addAction("echobody", [](HttpData& data) {
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = data.getRequest().getJson();
    });

    svr->addActionAndRegister<Simple>("/simple",
                                      { HttpMethod::GET,
                                        HttpMethod::POST });

    svr->addActionAndRegister<Simple>("/simple",
                                      { "put", "patch" });

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
