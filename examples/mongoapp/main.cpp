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

  try
  {
    // INCOMPLETE ---
    // TODO: All of this needs to be moved into an organized wrapper.
    mongo::client::initialize();
    mongo::DBClientConnection c;
    // TODO: Include this as a config value.
    c.connect("localhost");

    // Took some of this from the tutorials online.
    BSONObjBuilder b;
    b.append("name", "Joe");
    b.append("age", 33);

    BSONObj p = b.obj();
    c.insert("tutorial.persons", p);

    LOG_WARN(c.getLastError().c_str());
  }
  catch(const mongo::DBException &e)
  {
    LOG_ERROR(e.what());
  }

  QCoreApplication app(argc, argv);

  // Always initialize in the main thread.
  HttpServer::getInstance();

  thread webSvr(HttpServer::start);
  webSvr.detach();

  auto result = app.exec();

  //mongo::client::shutdown();

  // TODO: Shutdown the webserver.
  return result;
}
