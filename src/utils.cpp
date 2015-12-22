#include "utils.h"

using namespace qttp;

LogTrace::LogTrace(QString f, quint32 l)
{
  function.swap(f);
  line = l;
  qDebug().noquote().nospace() << LOG_DATETIME.append("ENTER ") << function << ":" << line;
}

LogTrace::~LogTrace()
{
  qDebug().noquote().nospace() << LOG_DATETIME.append("EXIT  ") << function << ":" << line;
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

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);

  if(parseError.error != QJsonParseError::NoError)
  {
    throw std::runtime_error((path + ": " +
                              parseError.errorString()).toStdString());
  }

  if(doc.isNull())
  {
    return QJsonObject();
  }
  return doc.object();
}

Stats::Stats()
{
}

Stats::~Stats()
{
}

void Stats::increment(const QString& key)
{
  m_Statistics[key] = m_Statistics[key].toInt() + 1;
}

void Stats::setValue(const QString& key, const QVariant& value)
{
  m_Statistics[key] = value;
}
