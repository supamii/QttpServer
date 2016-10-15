#include "defaults.h"
#include "httpdata.h"

using namespace qttp;
using namespace std;

OptionsPreprocessor::OptionsPreprocessor() :
  Processor()
{
}

const char* OptionsPreprocessor::getName() const
{
  return "optionsPreprocessor";
}

void OptionsPreprocessor::preprocess(HttpData& data)
{
  if(data.getMethod() == HttpMethod::OPTIONS)
  {
    static const QList<pair<string, string> > defaultHeaders =
    {
      { "Content-Type", "application/json" },
      { "Access-Control-Allow-Headers", "Content-Type, Authorization" },
      { "Access-Control-Allow-Methods", "GET, POST, DELETE, PUT, PATCH, HEAD, OPTIONS, TRACE" },
      { "Access-Control-Allow-Origin", "*" },
      { "Server", QTTP_SERVER_VERSION }
    };

    auto & resp = data.getResponse();
    for(auto & header : defaultHeaders)
    {
      resp.set_header(header.first, header.second);
    }
    data.finishResponse(string());
  }
}
