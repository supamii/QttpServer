#include "httprequest.h"

using namespace std;
using namespace qttp;

HttpRequest::HttpRequest(native::http::request* req) :
    m_Assertion(req),
    m_Request(req),
    m_HttpUrl(req)
{
}

HttpRequest::~HttpRequest()
{
}
