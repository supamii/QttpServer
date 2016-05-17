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

const QString& HttpUrl::schema() const
{
  if(m_Schema.isNull())
  {
    m_Schema = QSharedPointer<QString>(new QString(m_Request->url().schema().c_str()));
  }
  return *(m_Schema.data());
}

const QString& HttpUrl::host() const
{
  if(m_Host.isNull())
  {
    m_Host = QSharedPointer<QString>(new QString(m_Request->url().host().c_str()));
  }
  return *(m_Host.data());
}

const QString& HttpUrl::path() const
{
  if(m_Path.isNull())
  {
    m_Path = QSharedPointer<QString>(new QString(m_Request->url().path().c_str()));
  }
  return *(m_Path.data());
}

const QString& HttpUrl::query() const
{
  if(m_Query.isNull())
  {
    m_Query = QSharedPointer<QString>(new QString(m_Request->url().query().c_str()));
  }
  return *(m_Query.data());
}

const QString& HttpUrl::fragment() const
{
  if(m_Fragment.isNull())
  {
    m_Fragment = QSharedPointer<QString>(new QString(m_Request->url().fragment().c_str()));
  }
  return *(m_Fragment.data());
}
