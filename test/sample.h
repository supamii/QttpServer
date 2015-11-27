#ifndef QTTPSAMPLE_H
#define QTTPSAMPLE_H

#include <QtCore>

#include <action.h>

namespace qttp
{

class Sample: public qttp::Action
{
  public:
    void onAction(HttpData& data)
    {
      data.getResponse().set_status(200);
      data.getResponse().set_header("Content-Type", "text/plain");
      data.getResponse().end("Sample C++ FTW\n");
    }

    std::string getActionName()
    {
      return "sample";
    }
};

}

#endif // QTTPSAMPLE_H
