#include "httpdata.h"

using namespace std;
using namespace native::http;
using namespace qttp;

HttpData::HttpData(request* req, response* resp):
    m_Request(req),
    m_Response(resp),
    m_Json(),
    m_IsFinished(false),
    m_ControlFlag(true)
{
  Q_ASSERT(m_Request != nullptr);
  Q_ASSERT(m_Response != nullptr);
}

HttpData::~HttpData()
{
}

request& HttpData::getRequest() const
{
  if(m_IsFinished || m_Request == nullptr)
  {
    // Prevent dereferencing null
    throw runtime_error("native::http::request is no longer valid");
  }
  return *m_Request;
}

response& HttpData::getResponse() const
{
  if(m_IsFinished || m_Response == nullptr)
  {
    // Prevent dereferencing null
    throw runtime_error("native::http::response is no longer valid");
  }
  return *m_Response;
}

QJsonObject& HttpData::getJson()
{
  return m_Json;
}

bool HttpData::finishResponse(const std::string& body)
{
  m_IsFinished = true;

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
  m_IsFinished = true;

  // TODO: We'll want to dynamically update the status code, 400, 500?

  m_Response->set_header("Content-Type", "application/json");

  QJsonDocument doc(json);
  QByteArray body = doc.toJson();
  m_Response->write(body.length(), body.data());
  return m_Response->close();
}

bool HttpData::isFinished() const
{
  return m_IsFinished;
}

void HttpData::setControlFlag(bool shouldContinue)
{
  m_ControlFlag = shouldContinue;
}

bool HttpData::getControlFlag() const
{
  return m_ControlFlag;
}
