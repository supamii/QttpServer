#include "httpresponse.h"

using namespace std;
using namespace qttp;

HttpResponse::HttpResponse(native::http::response* resp) :
    m_Assertion(resp),
    m_Response(resp)
{
}

HttpResponse::~HttpResponse()
{
}
