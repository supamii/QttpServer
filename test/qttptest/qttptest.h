#ifndef QTTPTEST_H
#define QTTPTEST_H

#include <QtTest>
#include <qttpserver>
#include <testutils.h>

namespace qttp
{

#ifdef QTTP_DEBUG_TESTS
#define TEST_TRACE LOG_TRACE;
#else
#define TEST_TRACE
#endif

class SampleAction : public Action
{
  public:
    void onAction(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject& json = data.getResponse().getJson();
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
      TEST_TRACE;
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = "Sample C++ FTW Get";
    }

    void onPost(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = "Sample C++ FTW Post";
    }

    void onPut(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject& json = data.getResponse().getJson();
      json["response"] = "Sample C++ FTW Put";
    }

    void onDelete(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject& json = data.getResponse().getJson();
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
      TEST_TRACE;
      QJsonObject& json = data.getResponse().getJson();
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
      TEST_TRACE;
      data.getResponse().getJson()["preprocess"] = true;
    }

    void postprocess(HttpData& data)
    {
      TEST_TRACE;
      data.getResponse().getJson()["postprocess"] = true;
    }
};

}

#endif // QTTPTEST_H
