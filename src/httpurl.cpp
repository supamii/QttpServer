#include "httpurl.h"

using namespace std;
using namespace qttp;

HttpUrl::HttpUrl(native::http::QttpRequest* request) :
  QTTP_INIT_ASSERT_MEMBER(request)
  m_Request(request),
  m_Schema(std::move(m_Request->url().schema())),
  m_Host(std::move(m_Request->url().host())),
  m_Path(std::move(m_Request->url().path())),
  m_Query(std::move(m_Request->url().query())),
  m_Fragment(std::move(m_Request->url().fragment()))
{
}

HttpUrl::~HttpUrl()
{
}

const QByteArray& HttpUrl::getSchema() const
{
  return m_Schema;
}

const QByteArray& HttpUrl::getHost() const
{
  return m_Host;
}

qint32 HttpUrl::getPort() const
{
  return m_Request->url().port();
}

const QByteArray& HttpUrl::getPath() const
{
  return m_Path;
}

const QByteArray& HttpUrl::getQuery() const
{
  return m_Query;
}

const QByteArray& HttpUrl::getFragment() const
{
  return m_Fragment;
}
