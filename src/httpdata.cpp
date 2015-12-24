#include "httpdata.h"
#include "utils.h"

using namespace std;
using namespace native::http;
using namespace qttp;

HttpData::HttpData(request* req, response* resp):
    m_Request(req),
    m_Response(resp),
    m_Query(),
    m_Json(),
    m_ControlFlag(None),
    m_Uid(QUuid::createUuid()),
    m_Time()
{
  Q_ASSERT(m_Request != nullptr);
  Q_ASSERT(m_Response != nullptr);
  m_Time.start();
}

HttpData::~HttpData()
{
}

request& HttpData::getRequest() const
{
  Q_ASSERT(!isFinished() && m_Request != nullptr);
  return *m_Request;
}

response& HttpData::getResponse() const
{
  Q_ASSERT(!isFinished() && m_Response != nullptr);
  return *m_Response;
}

QUrlQuery& HttpData::getQuery()
{
  return m_Query;
}

const QUrlQuery& HttpData::getQuery() const
{
  return m_Query;
}

void HttpData::setQuery(QUrlQuery& params)
{
  m_Query.swap(params);
}

const QUuid& HttpData::getUid() const
{
  return m_Uid;
}

QJsonObject& HttpData::getJson()
{
  return m_Json;
}

bool HttpData::finishResponse(const std::string& body)
{
  setControlFlag(Finished);

  // TODO: Handle error response codes and possibly infer other details.
  m_Response->write(body);
  return m_Response->close();
}

bool HttpData::finishResponse()
{
  return finishResponse(m_Json);
}

bool HttpData::finishResponse(const QJsonObject& json)
{
  LOG_TRACE;
  setControlFlag(Finished);

  // TODO: Errors detected should set the status code, 400, 500, etc

  m_Response->set_header("Content-Type", "application/json");

  QJsonDocument doc(json);
  QByteArray body = doc.toJson();
  m_Response->write(body.length(), body.data());
  return m_Response->close();
}

quint32 HttpData::getControlFlag() const
{
  return m_ControlFlag;
}

void HttpData::setTerminated()
{
  setControlFlag(Terminated);
}

void HttpData::setControlFlag(eControl shouldContinue)
{
  m_ControlFlag |= shouldContinue;
}

bool HttpData::isFinished() const
{
  return m_ControlFlag & Finished;
}

bool HttpData::isTerminated() const
{
  return m_ControlFlag & Terminated;
}

bool HttpData::shouldContinue() const
{
  return !(m_ControlFlag & (Finished | Terminated));
}

bool HttpData::isProcessed() const
{
  return m_ControlFlag & (Preprocessed | Postprocessed | ActionProcessed);
}

void HttpData::setTimestamp(const QDateTime& timestamp)
{
  m_Timestamp = timestamp;
}

const QDateTime& HttpData::getTimestamp() const
{
  return m_Timestamp;
}

const QTime& HttpData::getTime() const
{
  return m_Time;
}
