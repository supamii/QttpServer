#ifndef QTTPHTTPROUTE_H
#define QTTPHTTPROUTE_H

#include "qttp_global.h"

namespace qttp
{

//! Usually typedefs can be a pain to track but this seems justified.
typedef std::pair<qttp::HttpMethod, QString> HttpPath;

class QTTPSHARED_EXPORT Input
{
  public:

    Input();
    Input(Input&& from);
    Input(const Input& from);
    Input(const QString& name, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());
    Input(const QString& name, const QString& dataType, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());
    Input(const QString& name, const QString& dataType, const QString& desc, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());
    Input(const QString& name, const QString& desc, const QStringList& values, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());

    Input& operator=(const Input& from);

    QString name;

    QString description;

    bool isRequired;

    //! SwaggerUI: Should match "in"
    QString paramType;

    //! SwaggerUI: Should match "type"
    QString dataType;

    //! SwaggerUI: Should match "enum"
    QStringList values;

    //! Primarily used to indicate if this route should be listed publicly.
    Visibility visibility;

    //! This Input applies only to the specified paths/routes
    std::set<qttp::HttpPath> paths;
};

class QTTPSHARED_EXPORT RequiredInput : public Input
{
  public:
    RequiredInput();
    RequiredInput(const QString&name, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());
    RequiredInput(const QString& name, const QString& desc, const QStringList& values, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());
};

class QTTPSHARED_EXPORT HeaderInput : public Input
{
  public:
    HeaderInput();
    HeaderInput(const QString&name, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());
    HeaderInput(const QString& name, const QString& desc, const QStringList& values, const std::set<qttp::HttpPath>& paths = std::set<qttp::HttpPath>());
};

/**
 * @brief The Route class
 *
 * TODO: Should this be a simple struct? Change later unless we decide to
 * leverage vtable.
 */
class QTTPSHARED_EXPORT Route
{
  public:

    Route() : action(), method(HttpMethod::UNKNOWN), path(), parts(), visibility(Visibility::Show)
    {
    }

    Route(const QString& actionName, const HttpPath& httpPath, Visibility visibility = Visibility::Show) :
      Route(actionName, httpPath.first, httpPath.second, visibility)
    {
    }

    Route(const QString& actionName, HttpMethod routeMethod, const QString& routePath, Visibility visibility = Visibility::Show) :
      action(actionName),
      method(routeMethod),
      path(routePath.startsWith('/') ? routePath : "/" + routePath),
      parts(routePath.split('/', QString::SkipEmptyParts)),
      visibility(visibility)
    {
    }

    Route(Route&& from) :
      action(std::move(from.action)),
      method(from.method),
      path(std::move(from.path)),
      parts(std::move(from.parts)),
      visibility(from.visibility)
    {
    }

    Route(const Route& from)
    {
      action = from.action;
      method = from.method;
      path = from.path;
      parts = from.parts;
      visibility = from.visibility;
    }

    Route& operator=(const Route& from)
    {
      action = from.action;
      method = from.method;
      path = from.path;
      parts = from.parts;
      visibility = from.visibility;
      return *this;
    }

    QString action;
    HttpMethod method;
    QString path;
    QStringList parts;
    Visibility visibility;
};

} // End namespace qttp

#endif // QTTPHTTPROUTE_H
