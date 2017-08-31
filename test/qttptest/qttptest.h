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
      QJsonObject json = data.getResponse().getJson().object();
      json["response"] = "Sample C++ FTW";
      data.getResponse().getJson().setObject(json);
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
      QJsonObject json = data.getResponse().getJson().object();
      json["response"] = "Sample C++ FTW Get";
      data.getResponse().getJson().setObject(json);
    }

    void onPost(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject json = data.getResponse().getJson().object();
      json["response"] = "Sample C++ FTW Post";
      data.getResponse().getJson().setObject(json);
    }

    void onPut(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject json = data.getResponse().getJson().object();
      json["response"] = "Sample C++ FTW Put";
      data.getResponse().getJson().setObject(json);
    }

    void onDelete(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject json = data.getResponse().getJson().object();
      json["response"] = "Sample C++ FTW Delete";
      data.getResponse().getJson().setObject(json);
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
      QJsonObject json = data.getResponse().getJson().object();
      json["response"] = "Sample C++ FTW With Parameter " + m_Param;
      data.getResponse().getJson().setObject(json);
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
      QJsonObject obj = data.getResponse().getJson().object();
      obj["preprocess"] = true;
      data.getResponse().getJson().setObject(obj);
    }

    void postprocess(HttpData& data)
    {
      TEST_TRACE;
      QJsonObject obj = data.getResponse().getJson().object();
      obj["postprocess"] = true;
      data.getResponse().getJson().setObject(obj);
    }
};

}

#endif // QTTPTEST_H
