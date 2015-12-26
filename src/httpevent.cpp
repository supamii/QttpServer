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

HttpEvent::HttpEvent(request* req, response* resp) :
    QEvent(QEvent::None),
    m_Request(req),
    m_Response(resp),
    m_Timestamp(QDateTime::currentDateTime())
{
}

HttpEvent::~HttpEvent()
{
}

request* HttpEvent::getRequest() const
{
  return m_Request;
}

response* HttpEvent::getResponse() const
{
  return m_Response;
}

const QDateTime& HttpEvent::getTimestamp() const
{
  return m_Timestamp;
}
