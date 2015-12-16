#include "utils.h"

using namespace qttp;

LogTrace::LogTrace(QString f, quint32 l)
{
  function.swap(f);
  line = l;
  qDebug().noquote().nospace() << LOG_DATETIME << " ENTER " << function << ":" << line;
}

LogTrace::~LogTrace()
{
  qDebug().noquote().nospace() << LOG_DATETIME << " EXIT  " << function << ":" << line;
}

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

