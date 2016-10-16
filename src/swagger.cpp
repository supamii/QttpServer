#include "swagger.h"
#include "httpserver.h"

using namespace std;
using namespace qttp;

const char* Swagger::ACTION_NAME = "swagger";

Swagger::Swagger() :
  Action(),
  m_IsEnabled(true),
  m_Response()
{
  initialize();
}

Swagger::~Swagger()
{
}

const char* Swagger::getName() const
{
  return ACTION_NAME;
}

const QList<pair<HttpMethod, QString> >& Swagger::getRoutes() const
{
  const static QList<pair<HttpMethod, QString> > list =
  {
    { HttpMethod::GET, "/swagger" }
  };
  return list;
}

void Swagger::initialize()
{
  HttpServer* svr = HttpServer::getInstance();

  m_IsEnabled = svr->isSwaggerEnabled();
  if(!m_IsEnabled)
  {
    return;
  }

  const HttpServer::ServerInfo& info = svr->getServerInfo();
  const QString& version = info.version;

  m_Response = QJsonObject {
    { "swagger", "2.0" },
    { "info", QJsonObject {
        { "title", info.title },
        { "description", info.description },
        { "version", info.version },
        { "termsOfService", info.termsOfService },
        { "contact", QJsonObject {
            { "email", info.contactEmail }
          }},
        { "license", QJsonObject {
            { "name", info.licenseName },
            { "url", info.licenseUrl }
          }},
        { "company", QJsonObject {
            { "name", info.companyName },
            { "url", info.companyUrl }
          }},
      }},
    { "host", info.host },
    { "basePath", info.basePath },
    { "schemes", info.schemes },
    { "consumes", info.consumes },
    { "produces", info.produces }
  };

  QJsonObject definitions;
  QJsonObject paths;
  QJsonObject parameters;

  const QHash<QString, shared_ptr<const Action> >& actions = svr->getActions();

  for(const auto & action : actions)
  {
    QJsonObject properties;

    QJsonArray refParams;
    QJsonArray required;
    QJsonArray tags;

    QString actionName = action->getName();
    const QList<Input> inputs = action->getInputs();

    for(const auto & input : inputs)
    {
      QJsonArray enums;
      for(int i = 0; i < input.enums.size(); ++i)
      {
        enums.append(input.enums.at(i));
      }

      QString paramKey = "action_" + actionName + version + "_" + input.name;

      parameters[paramKey] = QJsonObject {
        { "name", input.name },
        { "in", input.paramType },
        { "type", input.dataType },
        { "description", input.description },
        { "required", input.required },
        { "enum", enums.size() == 0 ? QJsonValue() : enums }
      };

      if(input.required)
      {
        required.append(input.name);
      }

      refParams.append(QJsonObject {
        { "$ref", "#/parameters/" + paramKey }
      });

      properties[input.name] = QJsonObject { { "type", "string" } };
    }

    definitions.insert(actionName + version, QJsonValue(QJsonObject {
      { "properties", properties }
    }));

    for(auto httpMethod : Global::HTTP_METHODS)
    {
      const QHash<QString, HttpServer::Route>& routes = svr->getRoutes(httpMethod);

      QJsonArray routeParameters(refParams);

      if(httpMethod == HttpMethod::POST ||
         httpMethod == HttpMethod::PUT ||
         httpMethod == HttpMethod::PATCH)
      {
        // TODO: Try and include the schema?

        routeParameters.append(QJsonObject {
          { "name", "body" },
          { "in", "body" },
          { "description", "Body of the action" },
          { "schema", QJsonObject { { "type", "object" } } }
        });
      }

      for(const auto & route : routes)
      {
        if(route.action != actionName)
        {
          continue;
        }
        if(!paths.contains(route.route))
        {
          paths.insert(route.route, QJsonObject());
        }
        QJsonObject pathRoute = paths[route.route].toObject();
        pathRoute.insert(Utils::toStringLower(httpMethod), QJsonObject {
          { "summary", action->getSummary() },
          { "description", action->getDescription() },
          { "operationId", route.route },
          { "parameters", routeParameters },
          { "tags", tags }
        });
        paths[route.route] = pathRoute;
      }
    }

  } // End for(const auto & action : action)

  //paths.insert(route.route, QJsonValue(path));

  m_Response["definitions"] = definitions;
  m_Response["paths"] = paths;
  m_Response["parameters"] = parameters;

}

void Swagger::onGet(HttpData& data)
{
  if(m_IsEnabled)
  {
    data.getResponse().setJson(m_Response);
  }
  else
  {
    // TODO: FIXME: Do something? At least finish the response.
  }
}
