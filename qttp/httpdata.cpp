#include "httpdata.h"

using namespace std;
using namespace native::http;
using namespace qttp;

HttpData::HttpData(request* req, response* resp): m_Request(req), m_Response(resp), m_Json()
{
}

HttpData::~HttpData()
{
}

request& HttpData::getRequest() const
{
  if(m_Request == nullptr)
  {
    stringstream ss;
    ss << __FUNCTION__ << " unexpected NULL ptr @ " << __LINE__;
    throw runtime_error(ss.str());
  }
  return *m_Request;
}

response& HttpData::getResponse() const
{
  if(m_Response == nullptr)
  {
    stringstream ss;
    ss << __FUNCTION__ << " unexpected NULL ptr @ " << __LINE__;
    throw runtime_error(ss.str());
  }
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
