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
  return "options";
}

void OptionsPreprocessor::preprocess(HttpData& data)
{
  if(data.getRequest().getMethod() == HttpMethod::OPTIONS)
  {
    // For now we won't use Global::getDefaultHeaders() until we can figure
    // out how to ensure the OPTIONS method will always work as expected.

    static const QList<QStringPair> defaultHeaders =
    {
      { "Content-Type", "application/json" },
      { "Access-Control-Allow-Headers", "Content-Type, Authorization" },
      { "Access-Control-Allow-Methods", "GET, POST, DELETE, PUT, PATCH, HEAD, OPTIONS, TRACE" },
      { "Access-Control-Allow-Origin", "*" },
      { "Server", QTTP_SERVER_VERSION }
    };

    auto& response = data.getResponse();
    response.setHeader(defaultHeaders);
    response.finish();
  }
}
