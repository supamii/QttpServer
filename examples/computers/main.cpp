#include <qttpserver>
#include <desktops.h>
#include <phones.h>
#include <tablets.h>
#include <laptops.h>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  // Always initialize in the main thread.
  qttp::HttpServer* httpSvr = qttp::HttpServer::getInstance();
  httpSvr->initialize();

  auto action = httpSvr->addAction<Desktops>();
  action->registerRoute(qttp::HttpMethod::GET, "desktops");
  action->registerRoute(qttp::HttpMethod::GET, "desktops/:model");
  action->registerRoute(qttp::HttpMethod::PUT, "desktops");

  httpSvr->addActionAndRegister<Phones>("phones",
                                        { qttp::HttpMethod::GET,
                                          qttp::HttpMethod::PUT });
  httpSvr->addActionAndRegister<Phones>("phones/:model",
                                        { qttp::HttpMethod::GET });

  httpSvr->addActionAndRegister<Tablets>();

  httpSvr->addActionAndRegister<Laptops>();

  qttp::HttpServer::ServerInfo info = httpSvr->getServerInfo();
  info.title = "Computers API";
  info.description = "An API that lists different types of computers.";
  info.companyName = "Computers";
  httpSvr->setServerInfo(info);

  httpSvr->initHttpDirectory(QDir().absoluteFilePath("./www"));

  // Libuv runs in its own thread.
  httpSvr->startServer();

  // Qt takes the main thread per the usual.
  return app.exec();
}
