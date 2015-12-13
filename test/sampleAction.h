#ifndef QTTPSAMPLE_H
#define QTTPSAMPLE_H

#include <QtCore>

#include "action.h"

namespace qttp
{

class SampleAction: public Action
{
  public:
    void onAction(HttpData& data)
    {
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW";
    }

    const std::string getActionName() const
    {
      return "sample";
    }
};

class SampleProcessor: public Processor
{
  public:
    const std::string getProcessorName() const
    {
      return "SampleProcessor";
    }

    void preprocess(HttpData& data)
    {
      data.getJson()["preprocess"] = true;
    }

    void postprocess(HttpData& data)
    {
      data.getJson()["postprocess"] = true;
    }
};

}

#endif // QTTPSAMPLE_H
