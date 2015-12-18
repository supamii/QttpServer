#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <thread>

#include <mongo/client/dbclient.h>

#include "httpserver.h"

using namespace std;
using namespace qttp;
using namespace native::http;
using namespace mongo;

int main(int argc, char** argv)
{
  LOG_TRACE;

  QCoreApplication app(argc, argv);

  // Always initialize in the main thread.
  HttpServer* httpSvr = HttpServer::getInstance();

  auto result = -1;

  mongo::client::initialize();
  mongo::DBClientConnection c;

  try
  {
    c.connect("localhost");

    // Registers the action "addPerson"
    // to the http method "post"
    // when the request url targets "http://localhost:8080/person"

    httpSvr->registerRoute("post", "addPerson", "/person");

    httpSvr->addAction("addPerson", [&](HttpData& data)
    {
      // Took some of this from the tutorials online.
      BSONObj p = BSON( "name" << "Joe" << "age" << 33 );
      c.insert("tutorial.persons", p);

      string err = c.getLastError();
      if(!err.empty())
      {
        LOG_WARN(err.c_str());
      }

      QJsonObject& json = data.getJson();
      json["response"] = QJsonDocument::fromJson(QString(p.jsonString(Strict).c_str()).toLatin1()).object();
    });

    // Registers the action "getPerson"
    // to the http method "get"
    // when the request url targets "http://localhost:8080/person"

    httpSvr->registerRoute("get", "getPerson", "/person");

    httpSvr->addAction("getPerson", [&](HttpData& data)
    {
      QJsonArray array;
      QJsonObject& json = data.getJson();
      auto_ptr<DBClientCursor> cursor = c.query("tutorial.persons", BSONObj());

      string err = c.getLastError();
      if(!err.empty())
      {
        LOG_WARN(err.c_str());
      }

      while (cursor->more())
      {
         BSONObj obj = cursor->next();
         string str = obj.jsonString(Strict);
         QByteArray bytes(str.c_str(), str.length());

         QJsonParseError error;
         QJsonObject entry = QJsonDocument::fromJson(bytes, &error).object();
         array.push_back(entry);
      }

      json["response"] = array;
    });

    thread webSvr(HttpServer::start);
    webSvr.detach();

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

  // TODO: Shutdown the webserver.
  return result;
}
