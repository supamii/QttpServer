#include "httpevent.h"

using namespace std;
using namespace native::http;

HttpEvent::HttpEvent() : QEvent(QEvent::None), data()
{
}

HttpEvent::HttpEvent(pair<request*, response*> d) :  QEvent(QEvent::None), data(d)
{
}

HttpEvent::HttpEvent(request* req, response* resp) : QEvent(QEvent::None), data(pair<request*, response*>(req, resp))
{
}

HttpEvent::~HttpEvent()
{

}

void HttpEvent::setData(const pair<request*, response*>& data)
{
    this->data = data;
}

pair<request*, response*>& HttpEvent::getData()
{
    return this->data;
}

pair<request*, response*> HttpEvent::getData() const
{
    return this->data;
}
