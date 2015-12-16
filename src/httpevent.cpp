#include "../src/httpevent.h"

using namespace std;
using namespace qttp;
using namespace native::http;

HttpEvent::HttpEvent() :
    QEvent(QEvent::None),
    m_Data()
{
}

HttpEvent::HttpEvent(pair<request*, response*> d) :
    QEvent(QEvent::None),
    m_Data(d)
{
}

HttpEvent::HttpEvent(request* req, response* resp) :
    QEvent(QEvent::None),
    m_Data(pair<request*, response*>(req, resp))
{
}

HttpEvent::~HttpEvent()
{
}

void HttpEvent::setData(const pair<request*, response*>& data)
{
  this->m_Data = data;
}

pair<request*, response*>& HttpEvent::getData()
{
  return this->m_Data;
}

pair<request*, response*> HttpEvent::getData() const
{
  return this->m_Data;
}
