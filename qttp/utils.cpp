#include "utils.h"

using namespace qttp;

Utils::Utils()
{
}

Utils::~Utils()
{
}

QJsonObject Utils::readJson(const QString& path)
{
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly))
  {
    return QJsonObject();
  }
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if(doc.isNull())
  {
    return QJsonObject();
  }
  return doc.object();
}

