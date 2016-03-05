#include "action.h"

using namespace std;
using namespace qttp;

Action::Action()
{
}

Action::~Action()
{
}

void Action::onAction(HttpData &data)
{
  string method = data.getRequest().get_method();

  if (method == "GET")
  {
    this->onGet(data);
  }
  else if (method == "POST")
  {
    this->onPost(data);
  }
  else if(method == "PUT")
  {
    this->onPut(data);
  }
  else if(method == "DELETE")
  {
    this->onDelete(data);
  }
  else
  {
    this->onUnknown(data);
  }
}

void Action::onGet(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onPost(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onPut(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onDelete(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onUnknown(HttpData& data)
{
  Q_UNUSED(data);
}

Processor::Processor()
{
}

Processor::~Processor()
{
}

void Processor::preprocess(HttpData& data)
{
  Q_UNUSED(data);
}

void Processor::postprocess(HttpData& data)
{
  Q_UNUSED(data);
}
