#include "httprequest.h"

using namespace std;
using namespace qttp;

HttpRequest::HttpRequest(native::http::request* req) :
    m_Assertion(req),
    m_Request(req),
    m_HttpUrl(),
    m_Method(),
    m_Body()
{
}

HttpRequest::~HttpRequest()
{
}

const native::http::url_obj& HttpRequest::url() const
{
  return m_Request->url();
}

const std::string& HttpRequest::get_header(const std::string& key) const
{
  return m_Request->get_header(key);
}

bool HttpRequest::get_header(const std::string& key, std::string& value) const
{
  return m_Request->get_header(key, value);
}

std::string HttpRequest::get_body() const
{
  return m_Request->get_body();
}

std::stringstream& HttpRequest::get_raw_body()
{
  return m_Request->get_raw_body();
}

const std::string& HttpRequest::get_method() const
{
  return m_Request->get_method();
}

uint64_t HttpRequest::get_timestamp() const
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

const QString& HttpRequest::getBody() const
{
  if(m_Body.isNull())
  {
    m_Body = QSharedPointer<QString>(new QString(get_body().c_str()));
  }
  return *(m_Body.data());
}

const QString& HttpRequest::getMethod() const
{
  if(m_Method.isNull())
  {
    m_Method = QSharedPointer<QString>(new QString(get_method().c_str()));
  }
  return *(m_Method.data());
}
