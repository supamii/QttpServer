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

  auto result = -1;

  // INCOMPLETE ---
  // TODO: All of this needs to be moved into an organized wrapper.
  mongo::client::initialize();
  mongo::DBClientConnection c;

  try
  {
    // TODO: Include this as a config value.
    c.connect("localhost");

    // Took some of this from the tutorials online.
    BSONObjBuilder b;
    b.append("name", "Joe");
    b.append("age", 33);

    BSONObj p = b.obj();
    c.insert("tutorial.persons", p);

    LOG_WARN(c.getLastError().c_str());

    auto_ptr<DBClientCursor> cursor = c.query("tutorial.persons", BSONObj());

    while (cursor->more())
    {
       LOG_DEBUG(cursor->next().toString().c_str());
    }

    LOG_WARN(c.getLastError().c_str());

    QCoreApplication app(argc, argv);

    // Always initialize in the main thread.
    HttpServer* httpSvr = HttpServer::getInstance();

    httpSvr->addAction("getPerson", [&](HttpData& data)
    {
      QJsonArray array;
      QJsonObject& json = data.getJson();
      auto_ptr<DBClientCursor> cursor = c.query("tutorial.persons", BSONObj());

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

    httpSvr->registerRoute("get", "getPerson", "/");

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
