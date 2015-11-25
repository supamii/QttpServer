#ifndef SAMPLE_H
#define SAMPLE_H

#include <QtCore>

#include <action.h>

class Sample: public qttp::Action
{
  public:
    Sample();
    virtual ~Sample();

    void onAction(native::http::request* req, native::http::response* resp)
    {
      resp->set_status(200);
      resp->set_header("Content-Type", "text/plain");
      resp->end("Sample C++ FTW");
    }

    std::string getActionName()
    {
      return "sample";
    }
};

#endif // SAMPLE_H
