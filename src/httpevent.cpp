#include "httpevent.h"

using namespace std;
using namespace qttp;
using namespace native::http;

HttpEvent::HttpEvent() :
  QEvent(QEvent::None),
  m_Request(nullptr),
  m_Response(nullptr),
  m_Timestamp()
{
}

HttpEvent::HttpEvent(QttpRequest* req, QttpResponse* resp) :
  QEvent(QEvent::None),
  m_Request(req),
  m_Response(resp),
  m_Timestamp(QDateTime::currentDateTime())
{
}

HttpEvent::~HttpEvent()
{
}

QttpRequest* HttpEvent::getRequest() const
{
  return m_Request;
}

QttpResponse* HttpEvent::getResponse() const
{
  return m_Response;
}

const QDateTime& HttpEvent::getTimestamp() const
{
  return m_Timestamp;
}

NativeHttpEvent::NativeHttpEvent() :
  QEvent(QEvent::None),
  m_Request(nullptr),
  m_Response(nullptr),
  m_Timestamp()
{
}

NativeHttpEvent::NativeHttpEvent(request* req, response* resp) :
  QEvent(QEvent::None),
  m_Request(req),
  m_Response(resp),
  m_Timestamp(QDateTime::currentDateTime())
{
}

NativeHttpEvent::~NativeHttpEvent()
{
}

request* NativeHttpEvent::getRequest() const
{
  return m_Request;
}

response* NativeHttpEvent::getResponse() const
{
  return m_Response;
}

const QDateTime& NativeHttpEvent::getTimestamp() const
{
  return m_Timestamp;
}
