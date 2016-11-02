#include <httpserver.h>
#include <mongo/client/dbclient.h>

using namespace std;
using namespace qttp;
using namespace native::http;
using namespace mongo;

int main(int argc, char** argv)
{
  LOG_TRACE;

  QCoreApplication app(argc, argv);

  HttpServer& httpSvr = *(HttpServer::getInstance());

  // Always initialize in the main thread.
  httpSvr.initialize();

  auto result = 0;

  mongo::client::initialize();
  mongo::DBClientConnection c;

  try
  {
    c.connect("localhost");

    // Registers the action "addPerson"
    // to the http method "post"
    // when the request url targets "http://localhost:8080/person"

    httpSvr.registerRoute("post", "addPerson", "/person");

    httpSvr.createAction("addPerson", [&](HttpData& data)
    {
      // Took some of this from the tutorials online.
      BSONObj p = BSON( "name" << "Joe" << "age" << 33 );
      c.insert("tutorial.persons", p);

      string err = c.getLastError();
      if(!err.empty())
      {
        LOG_WARN(err.c_str());
      }

      QByteArray bytes = Utils::toByteArray(p.jsonString(Strict));

      QJsonObject& json = data.getResponse().getJson();
      json["response"] = Utils::toJson(bytes);
    });

    // Registers the action "getPerson"
    // to the http method "get"
    // when the request url targets "http://localhost:8080/person"

    // Get all persons
    httpSvr.registerRoute("get", "getPerson", "/person");

    // Get a person by name e.g. http://localhost:8080/p/Joe
    httpSvr.registerRoute("get", "getPerson", "/p/:name");

    httpSvr.createAction("getPerson", [&](HttpData& data)
    {
      QJsonObject& json = data.getResponse().getJson();

      BSONObj bson;

      // Supports querying a person by name.
      QUrlQuery& params = data.getRequest().getQuery();
      if(params.hasQueryItem("name"))
      {
        bson = BSON("name" << params.queryItemValue("name").toStdString());
      }

      auto_ptr<DBClientCursor> cursor = c.query("tutorial.persons", bson);

      string err = c.getLastError();
      if(!err.empty())
      {
        LOG_WARN(err.c_str());
      }

      stringstream buffer;
      bool first = true;

      while (cursor->more())
      {
        buffer << (first ? "[" : ",");
        first = false;
        BSONObj obj = cursor->next();
        buffer << obj.jsonString(Strict);
      }

      buffer << "]";

      QByteArray bytes = Utils::toByteArray(buffer);
      QJsonParseError error;
      json["response"] = Utils::toArray(bytes, &error);

      if(error.error != QJsonParseError::NoError)
      {
        LOG_ERROR(error.errorString());
        // json["error"] = error.errorString();
      }
    });

    httpSvr.startServer();

    result = app.exec();

    //mongo::client::shutdown();
  }
  catch(const mongo::DBException &e)
  {
    LOG_ERROR(e.what());
  }
  catch(const exception& e)
  {
    LOG_ERROR(e.what());
  }

  return result;
}
