#include "httpurl.h"

using namespace std;
using namespace qttp;

HttpUrl::HttpUrl(native::http::request* request) :
    m_Assertion(request),
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

const QString& HttpUrl::getSchema() const
{
  if(m_Schema.isNull())
  {
    m_Schema = QSharedPointer<QString>(new QString(m_Request->url().schema().c_str()));
  }
  return *(m_Schema.data());
}

const QString& HttpUrl::getHost() const
{
  if(m_Host.isNull())
  {
    m_Host = QSharedPointer<QString>(new QString(m_Request->url().host().c_str()));
  }
  return *(m_Host.data());
}

qint32 HttpUrl::getPort() const
{
  return m_Request->url().port();
}

const QString& HttpUrl::getPath() const
{
  if(m_Path.isNull())
  {
    m_Path = QSharedPointer<QString>(new QString(m_Request->url().path().c_str()));
  }
  return *(m_Path.data());
}

const QString& HttpUrl::getQuery() const
{
  if(m_Query.isNull())
  {
    m_Query = QSharedPointer<QString>(new QString(m_Request->url().query().c_str()));
  }
  return *(m_Query.data());
}

const QString& HttpUrl::getFragment() const
{
  if(m_Fragment.isNull())
  {
    m_Fragment = QSharedPointer<QString>(new QString(m_Request->url().fragment().c_str()));
  }
  return *(m_Fragment.data());
}
