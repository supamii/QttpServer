#include "httprequest.h"

using namespace std;
using namespace qttp;

HttpRequest::HttpRequest(native::http::request* req) :
  m_Assertion(req),
  m_Request(req),
  m_HttpUrl(),
  m_MethodStr(),
  m_MethodEnum(HttpMethod::UNKNOWN),
  m_Json(),
  m_Query()
{
}

HttpRequest::~HttpRequest()
{
}

bool HttpRequest::containsHeader(const std::string& key) const
{
  auto headers = m_Request->get_headers();
  return headers.find(key) != headers.end();
}

const std::string& HttpRequest::getHeader(const std::string& key) const
{
  return m_Request->get_header(key);
}

bool HttpRequest::getHeader(const std::string& key, std::string& value) const
{
  return m_Request->get_header(key, value);
}

uint64_t HttpRequest::getTimestamp() const
{
  return m_Request->get_timestamp();
}

const HttpUrl& HttpRequest::getUrl() const
{
  if(m_HttpUrl.isNull())
  {
    m_HttpUrl = QSharedPointer<HttpUrl>(new HttpUrl(m_Request));
  }
  return *(m_HttpUrl.data());
}

const QString& HttpRequest::getMethodStr() const
{
  if(m_MethodStr.isNull())
  {
    m_MethodStr = QSharedPointer<QString>(new QString(m_Request->get_method().c_str()));
  }
  return *(m_MethodStr.data());
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

const std::stringstream& HttpRequest::getRawBody() const
{
  return m_Request->get_raw_body();
}

native::http::request* HttpRequest::getRequest()
{
  return m_Request;
}

const QJsonObject& HttpRequest::getJson() const
{
  if(!m_Json.isEmpty())
  {
    return m_Json;
  }

  // TODO: MUTEX!
  // FIXME: MUTEX!

  string body = m_Request->get_body();
  auto openBrace = body.find("{");
  auto closeBrace = body.find("}");

  if(openBrace != string::npos && openBrace == 0 &&
     closeBrace != string::npos && closeBrace == (body.length() - 1))
  {
    QJsonParseError error;
    m_Json = Utils::toJson(body, &error);

    if(error.error != QJsonParseError::NoError)
    {
      LOG_ERROR(error.errorString());
      m_Json = Utils::toJson(string("{}"));
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
