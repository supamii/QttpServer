# Quick Links

* The simplest example can be found under the project, [helloworld](./helloworld/).
* Want to hook your API into [SwaggerUI](./SWAGGER.md)?

# Tutorial - An Introduction to QttpServer

Find the example project for this tutorial [here](./computers/).

## Actions

A basic component of QttpServer is `qttp::Action`.  In this tutorial we'll extend this class 
and associate URL paths to demonstrate how the routing mechanisms work.

*Desktops.h:*

``` c++
#include <action.h>

class Desktops : public qttp::Action
{
  public:

    QStringList desktops;

    Desktops() : Action(), desktops({
    "iMac", "inspiron", "z800"
  })
    {
    }

    const char* getName() const
    {
      return "desktop-action";
    }
```

Begin by defining a `Desktops` class that extends from `qttp::Action`.  Here, the method 
`const char* getName() const` is the only pure virtual method that is required 
in all subclasses.  The name returned will be used as a unique identifier for 
the action, so take special care not to define actions that share the same name.

``` c++
    void onGet(qttp::HttpData& data)
    {
      qttp::HttpResponse& response = data.getResponse();
      QJsonObject& json = response.getJson();
      json["list"] = QJsonArray::fromStringList(desktops);
    }
```

Continue by implementing `void onGet(qttp::HttpData& data)` which, will be the 
entry point for all GET requests. For convenience, `qttp::HttpResponse` provides 
a reference to an empty `QJsonObject` that can be used to store the JSON response.

Simply populate `QJsonObject` and the framework will handle the rest. By default 
all responses will be returned with HTTP status 200.

``` c++
    void onPut(qttp::HttpData& data)
    {
      bool isSuccessful = false;
      const QJsonObject& request = data.getRequest().getJson();
      QJsonValue type = request["model"];
      if(type.isString())
      {
        desktops.append(type.toString());
        isSuccessful = true;
      }

      QJsonObject& response = data.getResponse().getJson();
      response["list"] = QJsonArray::fromStringList(desktops);
      response["success"] = isSuccessful;

      data.getResponse().setStatus(isSuccessful ?
                                   qttp::HttpStatus::OK :
                                   qttp::HttpStatus::BAD_REQUEST);
    }
```

Finally we handle the entry point for PUT requests that creates a new entry in our list of
desktop models/types.  For this PUT request, we're expecting an input parameter named `"model"`.
The `"model"` parameter may be set in either the query string, e.g. `desktops?model=pavillion`
or in the body of the PUT request, e.g. `{"model":"pavillion"}`.

Intended to be provided for convenience, the call to `HttpRequest::getJson()` will aggregate 
all input parameters from the query string as well as the body.

## Routes

*main.cpp:*

``` c++
#include <qttpserver>
#include <desktops.h>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  qttp::HttpServer* httpSvr = qttp::HttpServer::getInstance();
  httpSvr->initialize();

  auto action = httpSvr->addAction<Desktops>();
  action->registerRoute(qttp::HttpMethod::GET, "desktops");
  action->registerRoute(qttp::HttpMethod::PUT, "desktops");

  httpSvr->startServer();
  return app.exec();
}
```

`qttp::HttpServer::addAction()` starts off by adding and creating an Action.  Complete the setup by invoking 
`Action::registerRoute()` and then providing the http method, e.g. `PUT` `GET` `POST`, 
and finally the URL path `"desktops"`.

In this case, an http `GET` on `localhost:8080/desktops` will route to `void onGet(qttp::HttpData& data)`.

Similarly, an http `PUT` on `localhost:8080/desktops` will route to `void onPost(qttp::HttpData& data)`.

## Check it out!

``` bash
$ curl localhost:8080/desktops
```

`{"list":["iMac","inspiron","z800"]}`

## More Routes

There's more we can do with our routes!  Let's create a route that will return information about a 
computer desktop model.

*main.cpp:*

``` c++
  auto action = httpSvr->addAction<Desktops>();
  action->registerRoute(qttp::HttpMethod::GET, "desktops");
  action->registerRoute(qttp::HttpMethod::PUT, "desktops");
  // New route for getting a specific desktop model!
  action->registerRoute(qttp::HttpMethod::GET, "desktops/:model");
```

Add a new route with the path, `"desktops/:model"`.  This will describe a route that treats `":model"` 
as an input parameter.  

This will allow for a clean API:

* localhost:8080/`desktops/iMac`
* localhost:8080/`desktops/inspiron`
* localhost:8080/`desktops/z800`

``` c++
    void onGet(qttp::HttpData& data)
    {
      static const QHash<QString, QString> info = {{"iMac", "apple"}, {"inspiron", "dell"}, {"z800", "hp"}};
      QJsonValue type = data.getRequest().getJson()["model"];
      if(type.isString() && info.contains(type.toString()))
      {
        data.getResponse().getJson()["maker"] = info[type.toString()];
        return;
      }
      qttp::HttpResponse& response = data.getResponse();
      QJsonObject& json = response.getJson();
      json["list"] = QJsonArray::fromStringList(desktops);
    }
```

In this contrived example, we've added a hash-map that will accept the `"model"` parameter extracted from 
the request.  If it exists, we'll return information about the desktop model.

``` bash
$ curl localhost:8080/desktops/iMac
```
`{"maker":"apple"}`

``` bash
$ curl localhost:8080/desktops/inspiron
```
`{"maker":"dell"}`

``` bash
$ curl localhost:8080/desktops/z800
```
`{"maker":"hp"}`


## Action Scoped Routes

*laptops.h:*

``` c++
class Laptops : public qttp::Action
{
```

The preferred way to associate routes with an action is by reimplementing
`std::set<qttp::HttpPath> getRoutes() const`.  We return a list of `HttpPath`
objects that specify pairs of `HttpMethod` and URL path.

``` c++
    std::set<qttp::HttpPath> getRoutes() const
    {
      return {
               { qttp::HttpMethod::GET, "laptops" },
               { qttp::HttpMethod::GET, "laptops/:model" },
               { qttp::HttpMethod::GET, "laptops/:model/:color" },
               { qttp::HttpMethod::PUT, "laptops" },
      };
    }
```

After that, make a single call from to `addActionAndRegister()` to setup
the action!

*main.cpp:*

``` c++
#include <qttpserver>
#include <desktops.h>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);

  qttp::HttpServer* httpSvr = qttp::HttpServer::getInstance();
  httpSvr->initialize();

  httpSvr->addActionAndRegister<Laptops>();
```
