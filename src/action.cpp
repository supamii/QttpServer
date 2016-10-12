#include "action.h"

using namespace std;
using namespace qttp;

const QList<Input> Action::m_EmptyInputList;
const QStringList Action::m_EmptyTagList;

Action::Action()
{
}

Action::~Action()
{
}

void Action::onAction(HttpData &data)
{
  HttpMethod method = Utils::fromStdString(data.getRequest().get_method());
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

    case HttpMethod::DELETE:
      this->onDelete(data);
      break;

    case HttpMethod::PATCH:
      this->onPatch(data);
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

void Action::onDelete(HttpData& data)
{
  Q_UNUSED(data);
}

void Action::onUnknown(HttpData& data)
{
  Q_UNUSED(data);
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
  return m_EmptyTagList;
}

const QList<Input>& Action::getInputs() const
{
  return m_EmptyInputList;
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

Input::Input(const QString& n, const QStringList& e) :
  name(n),
  description(),
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

RequiredInput::RequiredInput(const QString& n, const QStringList& e) :
  Input(n, e)
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
