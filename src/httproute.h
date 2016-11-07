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

    Route() : action(), path(), parts(), visibility(Visibility::Show)
    {
    }

    Route(const QString& actionName, const QString& routePath, Visibility visibility = Visibility::Show) :
      action(actionName),
      path(routePath.startsWith('/') ? routePath : "/" + routePath),
      parts(routePath.split('/', QString::SkipEmptyParts)),
      visibility(visibility)
    {
    }

    Route(Route&& from) :
      action(std::move(from.action)),
      path(std::move(from.path)),
      parts(std::move(from.parts)),
      visibility(from.visibility)
    {
    }

    Route(const Route& from)
    {
      action = from.action;
      path = from.path;
      parts = from.parts;
      visibility = from.visibility;
    }

    Route& operator=(const Route& from)
    {
      action = from.action;
      path = from.path;
      parts = from.parts;
      visibility = from.visibility;
      return *this;
    }

    QString action;
    QString path;
    QStringList parts;
    Visibility visibility;
};

} // End namespace qttp

#endif // QTTPHTTPROUTE_H
