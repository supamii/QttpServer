#include "httpresponse.h"

using namespace std;
using namespace qttp;
using namespace native::http;

HttpResponse::HttpResponse(response* resp) :
  m_Assertion(resp),
  m_Response(resp),
  m_Status(HttpStatus::OK),
  m_Json(),
  m_ControlFlag(DataControl::None)
{
}

HttpResponse::~HttpResponse()
{
}

const native::http::response* HttpResponse::getResponse()
{
  return m_Response;
}

void HttpResponse::setHeader(const string& key, const string& value)
{
  m_Response->set_header(key, value);
}

void HttpResponse::setHeader(const QList<std::pair<std::string, std::string> >& headers)
{
  for(auto & header : headers)
  {
    setHeader(header.first, header.second);
  }
}

void HttpResponse::setStatus(HttpStatus status)
{
  m_Status = status;
  m_Response->set_status((int) m_Status);
}

HttpStatus HttpResponse::getStatus() const
{
  return m_Status;
}

QJsonObject& HttpResponse::getJson()
{
  return m_Json;
}

const QJsonObject& HttpResponse::getJson() const
{
  return m_Json;
}

void HttpResponse::setJson(const QJsonObject& json)
{
  m_Json = json;
}

bool HttpResponse::finish()
{
  if(m_Json.isEmpty())
  {
    return finish(string());
  }
  return finish(m_Json);
}

bool HttpResponse::finish(const string& body)
{
  LOG_TRACE;
  setFlag(DataControl::Finished);

  // TODO: Handle error response codes and possibly infer other details.
  m_Response->write(body);
  return m_Response->close();
}

bool HttpResponse::finish(const QJsonObject& json)
{
  LOG_TRACE;
  setHeader("Content-Type", "application/json");
  QJsonDocument doc(json);
  return finish(doc.toJson());
}

bool HttpResponse::finish(const QByteArray& bytes)
{
  setFlag(DataControl::Finished);
  m_Response->write(bytes.length(), bytes.data());
  return m_Response->close();
}

bool HttpResponse::isFinished() const
{
  return m_ControlFlag & DataControl::Finished;
}

bool HttpResponse::isProcessed() const
{
  return m_ControlFlag & (DataControl::Preprocessed | DataControl::Postprocessed | DataControl::ActionProcessed);
}

bool HttpResponse::shouldContinue() const
{
  return !(m_ControlFlag & (DataControl::Finished | DataControl::Terminated));
}

bool HttpResponse::isTerminated() const
{
  return m_ControlFlag & DataControl::Terminated;
}

void HttpResponse::setTerminated()
{
  setFlag(DataControl::Terminated);
}

quint32 HttpResponse::getFlag() const
{
  return m_ControlFlag;
}

void HttpResponse::setFlag(DataControl flag)
{
  m_ControlFlag |= static_cast<quint32>(flag);
}
