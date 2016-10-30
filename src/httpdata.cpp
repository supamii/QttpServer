#include "httpdata.h"
#include "utils.h"

using namespace std;
using namespace native::http;
using namespace qttp;

HttpData::HttpData(request* req, response* resp) :
  m_HttpRequest(req),
  m_HttpResponse(resp),
  m_Uid(QUuid::createUuid()),
  m_Time()
{
  m_Time.start();
}

HttpData::~HttpData()
{
}

const HttpRequest& HttpData::getRequest() const
{
  return m_HttpRequest;
}

HttpRequest& HttpData::getRequest()
{
  return m_HttpRequest;
}

const HttpResponse& HttpData::getResponse() const
{
  return m_HttpResponse;
}

HttpResponse& HttpData::getResponse()
{
  return m_HttpResponse;
}

void HttpData::setResponse(const QJsonObject& json)
{
  getResponse().setJson(json);
}

void HttpData::setErrorResponse(const QString& msg)
{
  setErrorResponse(msg, HttpError::BAD_REQUEST);
}

void HttpData::setErrorResponse(const QString &msg, HttpError code)
{
  getResponse().setStatus(static_cast<HttpStatus>(code));
  getResponse().getJson()["error"] = msg;
}

void HttpData::setErrorResponse(const QJsonObject& json)
{
  setErrorResponse(json, HttpError::BAD_REQUEST);
}

void HttpData::setErrorResponse(const QJsonObject &json, HttpError code)
{
  getResponse().setStatus(static_cast<HttpStatus>(code));
  getResponse().setJson(json);
}

const QUuid& HttpData::getUid() const
{
  return m_Uid;
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
