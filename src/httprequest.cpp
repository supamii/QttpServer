#include "httprequest.h"

using namespace std;
using namespace qttp;

HttpRequest::HttpRequest(native::http::QttpRequest* req) :
  QTTP_INIT_ASSERT_MEMBER(req)
  m_Request(req),
  m_HttpUrl(req),
  m_MethodEnum(HttpMethod::UNKNOWN),
  m_Json(),
  m_Query()
{
}

HttpRequest::~HttpRequest()
{
}

bool HttpRequest::containsHeader(const QString& key) const
{
  auto & headers = m_Request->get_headers();
  return headers.find(key) != headers.end();
}

const QString& HttpRequest::getHeader(const QString& key) const
{
  return m_Request->get_header(key);
}

bool HttpRequest::getHeader(const QString& key, QString& value) const
{
  return m_Request->get_header(key, value);
}

uint64_t HttpRequest::getTimestamp() const
{
  return m_Request->get_timestamp();
}

const HttpUrl& HttpRequest::getUrl() const
{
  return m_HttpUrl;
}

const QString& HttpRequest::getMethodStr() const
{
  return m_Request->get_method();
}

HttpMethod HttpRequest::getMethod(bool strictComparison) const
{
  if(m_MethodEnum == HttpMethod::UNKNOWN)
  {
    m_MethodEnum = strictComparison ?
                   Utils::fromString(getMethodStr()) :
                   Utils::fromPartialString(getMethodStr());
  }
  return m_MethodEnum;
}

native::http::QttpRequest* HttpRequest::getRequest()
{
  return m_Request;
}

const QJsonObject& HttpRequest::getJson() const
{
  if(!m_Json.isEmpty())
  {
    return m_Json;
  }

  const QByteArray& body = m_Request->get_body();
  auto openBrace = body.indexOf("{");
  auto closeBrace = body.lastIndexOf("}");

  if(openBrace >= 0 && closeBrace >= 0)
  {
    QJsonParseError error;
    m_Json = Utils::toJson(body, &error);

    if(error.error != QJsonParseError::NoError)
    {
      // This should probably be muted later.
      LOG_WARN(error.errorString());
      m_Json = Utils::toJson(QByteArray("{}"));
    }
  }

  QList<QPair<QString, QString> > list = getQuery().queryItems();
  for(auto i = list.begin(); i != list.end(); ++i)
  {
    m_Json.insert(i->first, i->second);
  }

  return m_Json;
}

QUrlQuery& HttpRequest::getQuery()
{
  return m_Query;
}

const QUrlQuery& HttpRequest::getQuery() const
{
  return m_Query;
}

void HttpRequest::setQuery(QUrlQuery& params)
{
  m_Query.swap(params);
}
