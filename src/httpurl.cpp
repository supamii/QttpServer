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
