#ifndef QTTPSAMPLE_H
#define QTTPSAMPLE_H

#include <QtCore>

#include <action.h>

namespace qttp
{

class Sample: public qttp::Action
{
  public:
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

}

#endif // QTTPSAMPLE_H
