#include "action.h"
#include "httpserver.h"

using namespace std;
using namespace qttp;

const QList<qttp::HttpPath> Action::m_EmptyRoutes;
const std::vector<Input> Action::m_EmptyInputList;
const QStringList Action::m_EmptyStringList;
const std::vector<QStringPair> Action::m_EmptyStringPairList;

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

QList<qttp::HttpPath> Action::getRoutes() const
{
  return m_EmptyRoutes;
}

const char* Action::getSummary() const
{
  return "";
}

const char* Action::getDescription() const
{
  return "";
}

QStringList Action::getTags() const
{
  return m_EmptyStringList;
}

std::vector<Input> Action::getInputs() const
{
  return m_EmptyInputList;
}

std::vector<QStringPair> Action::getHeaders() const
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

bool Action::registerRoute(HttpMethod method, const QString& path, Visibility visibility)
{
  HttpServer* svr = HttpServer::getInstance();
  return svr->registerRoute(method, Route(this->getName(), path, visibility));
}

bool Action::registerRoute(const qttp::HttpPath& path, Visibility visibility)
{
  HttpServer* svr = HttpServer::getInstance();
  return svr->registerRoute(path.first, Route(this->getName(), path.second, visibility));
}

void Action::registerRoute(const std::vector<qttp::HttpPath>& routes, Visibility visibility)
{
  HttpServer* svr = HttpServer::getInstance();
  for(const auto & path : routes)
  {
    svr->registerRoute(path.first, Route(this->getName(), path.second, visibility));
  }
}

SimpleAction::SimpleAction(std::function<void(qttp::HttpData&)> callback) :
  Action(),
  m_Callback(callback),
  m_Name(),
  m_Routes(),
  m_Summary(),
  m_Description(),
  m_Tags(),
  m_Inputs(),
  m_Headers()
{
}

SimpleAction::~SimpleAction()
{
}

void SimpleAction::onAction(HttpData &data)
{
  m_Callback(data);
}

void SimpleAction::setRoutes(const QList<qttp::HttpPath>& routes)
{
  m_Routes = routes;
}

QList<qttp::HttpPath> SimpleAction::getRoutes() const
{
  return m_Routes;
}

const char* SimpleAction::getName() const
{
  return m_Name.constData();
}

void SimpleAction::setSummary(const char* summary)
{
  m_Summary = summary;
}

const char* SimpleAction::getSummary() const
{
  return m_Summary.constData();
}

void SimpleAction::setDescription(const char* description)
{
  m_Description = description;
}

const char* SimpleAction::getDescription() const
{
  return m_Description.constData();
}

void SimpleAction::setTags(const QStringList& tags)
{
  m_Tags = tags;
}

QStringList SimpleAction::getTags() const
{
  return m_Tags;
}

void SimpleAction::setInputs(const std::vector<Input>& inputs)
{
  m_Inputs.insert(m_Inputs.end(), inputs.begin(), inputs.end());
}

std::vector<Input> SimpleAction::getInputs() const
{
  return m_Inputs;
}

std::vector<QStringPair> SimpleAction::getHeaders() const
{
  return m_Headers;
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
