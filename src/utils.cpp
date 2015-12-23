#include "utils.h"
#include "httpserver.h"

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

LoggingUtils::LoggingUtils() :
    QObject(),
    m_Mutex(),
    m_File(QDir::temp().absoluteFilePath(QString("qttp-").append(QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss")).append(".log"))),
    m_Stream(&m_File),
    m_OriginalMessageHandler(),
    m_TimerId(-1)
{
}

LoggingUtils::~LoggingUtils()
{
}

bool LoggingUtils::initializeFile(const QString& filename, quint32 flushDuration)
{
  if(!filename.isEmpty())
  {
    m_File.setFileName(filename);
  }

  if(m_File.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    LOG_DEBUG("Using log file" << m_File.fileName());
    m_OriginalMessageHandler = qInstallMessageHandler(LoggingUtils::fileLogger);

    // If our flush duration is 0 we intend to always flush on every write so
    // we'll check against m_TimerId's default value of -1.
    if(flushDuration > 0)
    {
      LOG_DEBUG("Flushing to log file every " << flushDuration << "ms");
      m_TimerId = this->startTimer(flushDuration, Qt::CoarseTimer);
    }
    else
    {
      LOG_DEBUG("Flush duration is not set, will flush on every single write");
    }

    return true;
  }

  LOG_WARN("Unable to open log file" << m_File.fileName());
  return false;
}

bool LoggingUtils::initializeSysLog()
{
  return false;
}

void LoggingUtils::timerEvent(QTimerEvent* event)
{
  if(event->timerId() != m_TimerId)
  {
    return;
  }

  LoggingUtils& log = HttpServer::getInstance()->getLoggingUtils();
  QMutexLocker scopedLock(&(log.m_Mutex));
  // We're periodically flushing the file in an effort to reduce overhead costs.
  log.m_Stream.flush();
}

void LoggingUtils::fileLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  LoggingUtils& log = HttpServer::getInstance()->getLoggingUtils();
  log.m_OriginalMessageHandler(type, context, msg);
  QMutexLocker scopedLock(&(log.m_Mutex));

  // If the timer hasn't been initialized then it means we need to flush on
  // every write.
  if(log.m_TimerId == -1)
  {
    log.m_Stream << msg << endl;
  }
  else
  {
  #ifdef Q_OS_WIN
    log.m_Stream << msg << '\n\r';
  #else
    log.m_Stream << msg << '\n';
  #endif
  }
}

