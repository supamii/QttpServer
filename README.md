# QttpServer

<b>QttpServer</b> is a fork from [node.native](https://github.com/d5/node.native) with some additional contributions from [tojocky](https://github.com/tojocky/node.native).  Intended as an alternative to [QHttpServer](https://github.com/nikhilm/qhttpserver), this is ideal for those who want the benefits of libuv with basic Qt libraries.

## Prerequisites

1. [git](http://git-scm.com/)
2. [python 2.x](https://www.python.org/)
3. [qt installer](http://www.qt.io/download/) or [from source](http://doc.qt.io/qt-5/linux-building.html)

## Start

```bash
git clone https://github.com/supamii/QttpServer
cd QttpServer
```

Git submodules/dependencies automatically pulls in mongodb-drivers, boost, libuv, http-parser
```base
git submodule update --init
```

Generate build files and compile to ./out/ folder
```base
./build.py
make -C out
```

Generate makefile and compile to ./out/qtdebug/ or ./out/qtrelease/ or launch `qttp.pro` with Qt Creator
```bash
qmake CONFIG+=debug qttp.pro
make
```

## Example

```c++
  HttpServer* httpSvr = HttpServer::getInstance();

  // Example 1 - Uses a raw std::function based callback.
  httpSvr->addAction("test", [](native::http::request*, native::http::response* resp) {
    resp->set_status(200);
    resp->set_header("Content-Type", "text/plain");
    resp->end("Test C++ FTW\n");
  });

  // Bind routes and actions together.
  httpSvr->registerRoute("/test", "test");
  httpSvr->registerRoute("/test2", "test");
```

```c++
  HttpServer* httpSvr = HttpServer::getInstance();

  // Example 2 - Uses the action interface.
  httpSvr->addAction<Sample>();

  httpSvr->registerRoute("/sample", "sample");
  httpSvr->registerRoute("/sampleAgain", "sample");

  class Sample : public Action {
    void onAction(native::http::request* req, native::http::response* resp) {
      resp->set_status(200);
      resp->set_header("Content-Type", "text/plain");
      resp->end("Sample C++ FTW");
    }
    std::string getActionName() { return "sample"; }
  };
```

## Optional components
##### Build MongoDb driver

1. Install [scons](http://www.scons.org/) - e.g. `brew install scons`
2. Install [boost](https://github.com/mongodb/mongo-cxx-driver/wiki/Download-and-Compile-the-Legacy-Driver) - e.g. `brew search boost`  `brew install homebrew/versions/boost155` Generally recommend using brew, apt-get, or the pre-built binary installer
3. Build the driver!
```bash
cd QttpServer/lib/mongo-cxx-driver
scons --libpath=/usr/local/opt/boost155/lib --cpppath=/usr/local/opt/boost155/include
# On mac contents may be under QttpServer/lib/mongo-cxx-driver/build/darwin/normal
```

For more information visit [mongodb.org](https://docs.mongodb.org/getting-started/cpp/client/)

