[![Build Status](https://travis-ci.org/supamii/QttpServer.svg?branch=master)](https://travis-ci.org/supamii/QttpServer)

# QttpServer 1.0.0

QttpServer focuses on developing a lean and mean C++ based API server. In addition to leveraging
modern C++, QttpServer also employs Qt to promote productivity and reinforce best practices. Use it
for systems that are resource constrained, general prototyping, integrate into existing Qt 
Applications, or if you simply wish to save a few bucks on your cloud servers.

**Getting started is EASY!**  Just load [qttpserver.pro](./qttpserver.pro) into [QtCreator](./img/qtcreator_project_setup_full.png)!

Check out the [examples](./examples/) and samples to get started with your RESTful API server!

<img src="/img/qttp_eventloops.png" alt="QttpServer" width="50%" height="50%">

## Features

* Compatible with [Qt](http://www.qt.io/download/) 5.5, 5.6, [5.7](http://download.qt.io/official_releases/qt/5.7/5.7.0/single/)
* NodeJS's blazing fast [libuv](https://github.com/libuv/libuv)
* NodeJS's lightning quick [http-parser](https://github.com/nodejs/http-parser)
* URL routing e.g. [/v1/your/api/path](./examples/README.md)
* Pre & Post processing hooks with chaining
* Submodules ready to support [MongoDb](https://github.com/mongodb/mongo-cxx-driver), [Redis](https://github.com/uglide/qredisclient) (see [build guide](./BUILD_OTHERS.md))
* Logging support
* [SwaggerUI](img/swagger.png) support and [integration](./examples/SWAGGER.md)
* Very basic support for HTML files (html, js, css, txt, etc)
* [Test Utilities](./test/) based on QtTest to keep you confident

## Example 1: 
Using a raw std::function based callback

```c++
#include <httpserver.h>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  // Always initialize in the main thread.
  qttp::HttpServer* httpSvr = qttp::HttpServer::getInstance();
  httpSvr->initialize();

  // Create an action, named "sayHello", that will handle all requests
  auto action = httpSvr->createAction([](qttp::HttpData& data) {
    // Form the JSON content and let the framework handle the rest.
    QJsonObject& json = data.getResponse().getJson();
    json["hello"] = "world";
  });

  // Register the action to handle http GET for the path "/".
  action->registerRoute(qttp::HttpMethod::GET, "/");

  // Register the action to handle http GET for the path "/hello".
  action->registerRoute(qttp::HttpMethod::GET, "/hello");

  // Libuv runs in its own thread.
  httpSvr->startServer();

  // Qt takes the main thread per the usual.
  return app.exec();
}
```

## Example 2: 
Using the action interface

```c++
#include <httpserver.h>

using namespace qttp;

class SayGoodbye : public Action {
  public:
    void onAction(HttpData& data) {
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = "adios";
    }
    const const char* getName() const { return "sayGoodbye"; }
};

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  HttpServer* httpSvr = HttpServer::getInstance();
  httpSvr->initialize();
  
  // Adds the action interface via template method.
  auto action = httpSvr->createAction<SayGoodbye>();
  
  action->registerRoute({
    {qttp::HttpMethod::GET, "/bye"},
    {qttp::HttpMethod::POST, "/aloha"}
  });

  httpSvr->startServer();
  return app.exec();
}
```

## SwaggerUI Integrated

<img src="/img/swagger.png" alt="SwaggerUI" width="50%" height="50%"><img src="/img/swagger_operations.png" alt="QttpServer Operations" width="50%" height="50%">

## **Getting started on Mac & Linux**

### **Prerequisites**

1. [git](http://git-scm.com/)
2. [qt installer](http://www.qt.io/download/) or [build](http://doc.qt.io/qt-5/linux-building.html) from [source](http://download.qt.io/official_releases/qt/5.7/5.7.0/single/).  Below I've successfully used this [tarball](http://download.qt.io/official_releases/qt/5.5/5.5.1/single/).

   ```bash
   # Building from source is a breeze for Ubuntu SERVER 12 & 14 LTS
   sudo apt-get install libssl-dev
   wget http://download.qt.io/official_releases/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.gz
   tar -xvf qt-everywhere-opensource-src-5.5.1.tar.tz
   cd qt-everywhere-opensource-src-5.5.1
   ./configure -nomake examples -opensource -skip qtwebkit -skip qtwebchannel -skip qtquick1 -skip qtdeclarative -openssl-linked
   make
   sudo make install
   ```

### **Setup**

```bash
git clone https://github.com/supamii/QttpServer.git
cd QttpServer
```

Git submodules/dependencies automatically pulls in mongodb-drivers, boost, libuv, http-parser
```bash
git submodule update --init
```

Using QtCreator, open `qttpserver.pro` and enjoy!

<img src="/img/qtcreator_project_setup_small.png" alt="QttpServer project loaded in QtCreator" width="30%" height="30%">

Using QMake:
```bash
qmake CONFIG+=debug qttpserver.pro
make
```

By default, qttpserver will compile as a static library for ease of use.  Check out the [examples here](./examples/) for a quick-start.  If you run into any blockers, feel free to open bug(s) and ask questions!

# More Resources

## Building Redis and MongoDB

Checkout the [guide](./BUILD_OTHERS.md).

## Examples and Tutorials

Riiiight over... [Here](./examples/)

## Alternate Build Scripts

node.native was previously built using **gyp** - you can learn more about that [here](./BUILD.md)

Note: It's likely that these will get phased out.

# Credits

`QttpServer` is a fork from [node.native](https://github.com/d5/node.native) with some additional contributions from [tojocky](https://github.com/tojocky/nodenative).

# License

The MIT License (MIT)

Copyright (c) 2016 Son-Huy Pham

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.