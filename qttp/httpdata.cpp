#include "httpdata.h"

using namespace std;
using namespace native::http;
using namespace qttp;

HttpData::HttpData(request* req, response* resp): m_Request(req), m_Response(resp), m_Json()
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
