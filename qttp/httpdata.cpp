#include "httpdata.h"

using namespace std;
using namespace native::http;
using namespace qttp;

HttpData::HttpData(request* req, response* resp): m_Request(req), m_Response(resp), m_Json(), m_IsFinished(false)
{
  Q_ASSERT(m_Request != nullptr);
  Q_ASSERT(m_Response != nullptr);
}

HttpData::~HttpData()
{
}

request& HttpData::getRequest() const
{
  return *m_Request;
}

response& HttpData::getResponse() const
{
  return *m_Response;
}

QJsonObject& HttpData::getJson()
{
  return m_Json;
}

const QJsonObject& HttpData::getJson() const
{
  return m_Json;
}

bool HttpData::finishResponse(const std::string& body)
{
  m_IsFinished = true;

  // TODO: Handle error response codes and possibly infer other details.
  return m_Response->end(body);
}

bool HttpData::finishJsonResponse()
{
  m_IsFinished = true;

  m_Response->set_status(200);
  m_Response->set_header("Content-Type", "application/json");

  QJsonDocument doc(m_Json);
  QByteArray body = doc.toJson();
  return m_Response->end(body.length(), body.data());
}

bool HttpData::isFinished() const
{
  return m_IsFinished;
}
