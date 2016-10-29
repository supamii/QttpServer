#include "qttp_global.h"

using namespace qttp;
using namespace std;

Global::Global()
{
}

const QList<HttpMethod> Global::HTTP_METHODS =
{
  HttpMethod::GET,
  HttpMethod::POST,
  HttpMethod::PUT,
  HttpMethod::PATCH,
  HttpMethod::HEAD,
  HttpMethod::DEL,
  HttpMethod::OPTIONS,
  HttpMethod::TRACE,
  HttpMethod::CONNECT
};

QList<std::pair<std::string, std::string> > Global::DEFAULT_HEADERS =
{
  { "Content-Type", "application/json" },
  { "Access-Control-Allow-Headers", "Content-Type, Authorization" },
  { "Access-Control-Allow-Methods", "GET, POST, DELETE, PUT, PATCH, HEAD, OPTIONS, TRACE" },
  { "Access-Control-Allow-Origin", "*" },
  { "Server", QTTP_SERVER_VERSION }
};

const QList<std::pair<std::string, std::string> >& Global::getDefaultHeaders()
{
  return DEFAULT_HEADERS;
}
