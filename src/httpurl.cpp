#include "httpurl.h"

using namespace std;
using namespace qttp;

HttpUrl::HttpUrl(native::http::QttpRequest* request) :
  QTTP_INIT_ASSERT_MEMBER(request)
  m_Request(request),
  m_Schema(),
  m_Host(),
  m_Path(),
  m_Query(),
  m_Fragment()
{
}

HttpUrl::~HttpUrl()
{
}

void HttpUrl::initialize()
{
  getSchema();
  getHost();
  getPath();
  getQuery();
  getFragment();
}

const QByteArray& HttpUrl::getSchema() const
{
  if(m_Schema.isNull())
  {
    m_Schema = std::move(m_Request->url().schema());
  }
  return m_Schema;
}

const QByteArray& HttpUrl::getHost() const
{
  if(m_Host.isNull())
  {
    m_Host = std::move(m_Request->url().host());
  }
  return m_Host;
}

qint32 HttpUrl::getPort() const
{
  return m_Request->url().port();
}

const QByteArray& HttpUrl::getPath() const
{
  if(m_Path.isNull())
  {
    m_Path = std::move(m_Request->url().path().constData());
  }
  return m_Path;
}

const QByteArray& HttpUrl::getQuery() const
{
  if(m_Query.isNull())
  {
    m_Query = std::move(m_Request->url().query().constData());
  }
  return m_Query;
}

const QByteArray& HttpUrl::getFragment() const
{
  if(m_Fragment.isNull())
  {
    m_Fragment = std::move(m_Request->url().fragment().constData());
  }
  return m_Fragment;
}
