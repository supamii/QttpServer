#include <native.h>
#include <QCoreApplication>
#include <QtCore>
#include <thread>

using namespace std;
using namespace native::http;

int startServer() {
    http server;
    int port = 8080;
    if(!server.listen("0.0.0.0", port, [](request& req, response& res) {
        QString url = QString::fromStdString(req.url().path());
        if(url.startsWith("/test")) {
          string body = req.get_body();
          // Now you can write a custom handler for the body content.
          res.set_status(200);
          res.set_header("Content-Type", "text/plain");
          res.end("C++ FTW\n");
          qDebug() << "Processed" << url;
        } else {
          res.set_status(400);
          res.set_header("Content-Type", "text/plain");
          res.end("Not allowed\n");
          qDebug() << "Rejected" << url;
        }
    })) return 1; // Failed to run server.
    qDebug() << "Server running at http://0.0.0.0:" << port << "/";
    return native::run();
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    std::thread webSvr(startServer);
    webSvr.detach();
    return app.exec();
}
