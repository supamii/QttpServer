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
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW";
      data.finishJsonResponse();
    }

    const std::string getActionName() const
    {
      return "sample";
    }
};

}

#endif // QTTPSAMPLE_H
