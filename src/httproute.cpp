#include "httproute.h"

using namespace qttp;

Input::Input() :
  name(),
  description(),
  isRequired(false),
  paramType("query"),
  dataType("string"),
  values(),
  visibility(Visibility::Show),
  paths()
{
}

Input::Input(Input&& from) :
  name(std::move(from.name)),
  description(std::move(from.description)),
  isRequired(from.isRequired),
  paramType(std::move(from.paramType)),
  dataType(std::move(from.dataType)),
  values(std::move(from.values)),
  visibility(Visibility::Show),
  paths(std::move(from.paths))
{
}

Input::Input(const Input& from)
{
  name = from.name;
  description = from.description;
  isRequired = from.isRequired;
  paramType = from.paramType;
  dataType = from.dataType;
  values = from.values;
  visibility = from.visibility;
  paths = from.paths;
}

Input::Input(const QString& inputname, const std::vector<qttp::HttpPath>& path) :
  name(inputname),
  description(),
  isRequired(false),
  paramType("query"),
  dataType("string"),
  values(),
  visibility(Visibility::Show),
  paths(path)
{
}

Input::Input(const QString& inputname, const QString& desc, const QStringList& vals, const std::vector<qttp::HttpPath>& path) :
  name(inputname),
  description(desc),
  isRequired(false),
  paramType("query"),
  dataType("string"),
  values(vals),
  visibility(Visibility::Show),
  paths(path)
{
}

Input& Input::operator=(const Input& from)
{
  name = from.name;
  description = from.description;
  isRequired = from.isRequired;
  paramType = from.paramType;
  dataType = from.dataType;
  values = from.values;
  visibility = from.visibility;
  paths = from.paths;
  return *this;
}

RequiredInput::RequiredInput() :
  Input()
{
}

RequiredInput::RequiredInput(const QString& inputname, const std::vector<qttp::HttpPath>& path) :
  Input(inputname, path)
{
  isRequired = true;
}

RequiredInput::RequiredInput(const QString& inputname, const QString& desc, const QStringList& vals, const std::vector<qttp::HttpPath>& path) :
  Input(inputname, desc, vals, path)
{
  isRequired = true;
}

HeaderInput::HeaderInput() :
  Input()
{
  paramType = "header";
}

HeaderInput::HeaderInput(const QString& inputname, const std::vector<qttp::HttpPath>& path) :
  Input(inputname, path)
{
  paramType = "header";
}

HeaderInput::HeaderInput(const QString& inputname, const QString& desc, const QStringList& vals, const std::vector<qttp::HttpPath>& path) :
  Input(inputname, desc, vals, path)
{
  paramType = "header";
}
