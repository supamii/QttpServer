#include "action.h"

using namespace std;
using namespace qttp;

const std::vector<Input> Action::m_EmptyInputList;
const QStringList Action::m_EmptyStringList;
const std::vector<std::pair<QString, QString> > Action::m_EmptyStringPairList;

Action::Action()
{
}

Action::~Action()
{
}

void Action::onAction(HttpData &data)
{
  HttpMethod method = data.getRequest().getMethod(false);
  switch(method)
  {
    case HttpMethod::GET:
      this->onGet(data);
      break;

    case HttpMethod::POST:
      this->onPost(data);
      break;

    case HttpMethod::PUT:
      this->onPut(data);
      break;

    case HttpMethod::DEL:
      this->onDelete(data);
      break;

    case HttpMethod::PATCH:
      this->onPatch(data);
      break;

    case HttpMethod::HEAD:
      this->onHead(data);
      break;

    case HttpMethod::OPTIONS:
      this->onOptions(data);
      break;

    case HttpMethod::TRACE:
      this->onTrace(data);
      break;

    case HttpMethod::CONNECT:
      this->onConnect(data);
      break;

    default:
      this->onUnknown(data);
      break;
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

void Action::onPatch(HttpData &data)
{
  Q_UNUSED(data);
}

void Action::onHead(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onDelete(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onOptions(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onTrace(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onConnect(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onUnknown(HttpData& data)
{
  Q_UNUSED(data);
}

const QList<pair<HttpMethod, QString> >& Action::getRoutes() const
{
  const static QList<pair<HttpMethod, QString> > list;
  return list;
}

const char* Action::getSummary() const
{
  return "";
}

const char* Action::getDescription() const
{
  return "";
}

const QStringList& Action::getTags() const
{
  return m_EmptyStringList;
}

const std::vector<Input>& Action::getInputs() const
{
  return m_EmptyInputList;
}

const std::vector<pair<QString, QString> >& Action::getHeaders() const
{
  return Global::getDefaultHeaders();
}

void Action::applyHeaders(HttpData& data) const
{
  auto& resp = data.getResponse();
  for(auto & header : getHeaders())
  {
    resp.setHeader(header.first, header.second);
  }
}

Input::Input() :
  name(),
  description(),
  required(false),
  paramType("query"),
  dataType("string"),
  enums()
{
}

Input::Input(const QString& n) :
  name(n),
  description(),
  required(false),
  paramType("query"),
  dataType("string"),
  enums()
{
}

Input::Input(const QString& n, const QString& desc) :
  name(n),
  description(desc),
  required(false),
  paramType("query"),
  dataType("string"),
  enums()
{
}

Input::Input(const QString& n, const QStringList& e) :
  name(n),
  description(),
  required(false),
  paramType("query"),
  dataType("string"),
  enums(e)
{
}

Input::Input(const QString& n, const QStringList& e, const QString& desc) :
  name(n),
  description(desc),
  required(false),
  paramType("query"),
  dataType("string"),
  enums(e)
{
}

RequiredInput::RequiredInput() :
  Input()
{
}

RequiredInput::RequiredInput(const QString& n) :
  Input(n)
{
  required = true;
}

RequiredInput::RequiredInput(const QString& n, const QString& desc) :
  Input(n, desc)
{
  required = true;
}

RequiredInput::RequiredInput(const QString& n, const QStringList& e) :
  Input(n, e)
{
  required = true;
}

RequiredInput::RequiredInput(const QString& n, const QStringList& e, const QString& desc) :
  Input(n, e, desc)
{
  required = true;
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
