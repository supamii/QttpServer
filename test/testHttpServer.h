#ifndef QTTPSAMPLE_H
#define QTTPSAMPLE_H

#include <QtTest/QtTest>

#include <qttpserver>
#include <action.h>
#include <utils.h>

namespace qttp
{

class SampleAction : public Action
{
  public:
    void onAction(HttpData& data)
    {
      LOG_TRACE;
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW";
    }

    const char* getName() const
    {
      return "sample";
    }
};

class SampleActionWithHttpMethods : public Action
{
  public:
    void onGet(HttpData& data)
    {
      LOG_TRACE;
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW Get";
    }

    void onPost(HttpData& data)
    {
      LOG_TRACE;
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW Post";
    }

    void onPut(HttpData& data)
    {
      LOG_TRACE;
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW Put";
    }

    void onDelete(HttpData& data)
    {
      LOG_TRACE;
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW Delete";
    }

    const char* getName() const
    {
      return "sampleWithHttpMethods";
    }
};

class ActionWithParameter : public Action
{
  public:
    ActionWithParameter(QString param) : Action(), m_Param(param)
    {
    }

    void onAction(HttpData& data)
    {
      LOG_TRACE;
      QJsonObject& json = data.getJson();
      json["response"] = "Sample C++ FTW With Parameter " + m_Param;
    }

    const char* getName() const
    {
      return "sampleWithParameter";
    }

    QString m_Param;
};

class SampleProcessor : public Processor
{
  public:
    const char* getName() const
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
