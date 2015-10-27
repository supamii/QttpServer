#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <thread>
#include "httpevent.h"
#include "httpeventhandler.h"

using namespace std;
using namespace native::http;

// Route mapping scheme
// Pre and Post processors

int startServer() {
    http server;
    auto port = 8080;

    // TODO: Ip interface should be configurable (later).

    auto result = server.listen("0.0.0.0", port, [](request& req, response& resp) {
        QString url = QString::fromStdString(req.url().path());
        if(url.startsWith("/test"))
        {
          qDebug() << "Sending event";

          HttpEvent* event = new HttpEvent(&req, &resp);
          QCoreApplication::postEvent(HttpEventHandler::getInstance(), event);

        } else {

          resp.set_status(400);
          resp.set_header("Content-Type", "text/plain");
          resp.end("Not allowed\n");
          qDebug() << "Rejected" << url;
        }
    });

    if(!result)
    {
        return 1;
    }

    qDebug() << "Server running at http://0.0.0.0:" << port << "/";
    return native::run();
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    // Make sure we initialize in the main thread.
    HttpEventHandler::getInstance();

    thread webSvr(startServer);
    webSvr.detach();

    return app.exec();
}
