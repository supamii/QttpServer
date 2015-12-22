#ifndef QTTPSAMPLE_H
#define QTTPSAMPLE_H

#include <QtCore>

#include <action.h>
#include <utils.h>

namespace qttp
{

class SampleAction: public Action
{
  public:
    void onAction(HttpData& data)
    {
      LOG_TRACE;
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW";
    }

    const QString getActionName() const
    {
      return "sample";
    }
};

class SampleProcessor: public Processor
{
  public:
    const QString getProcessorName() const
    {
      return "SampleProcessor";
    }

    void preprocess(HttpData& data)
    {
      LOG_TRACE;
      data.getJson()["preprocess"] = true;
    }

    void postprocess(HttpData& data)
    {
      LOG_TRACE;
      data.getJson()["postprocess"] = true;
    }
};

}

#endif // QTTPSAMPLE_H
