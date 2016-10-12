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

QttpException::QttpException(const std::string& message) :
  m_Message("QttpException: " + message)
{
}

const char* QttpException::what() const _NOEXCEPT
{
  return m_Message.c_str();
}

const QList<HttpMethod> Utils::HTTP_METHODS =
{
  HttpMethod::GET,
  HttpMethod::POST,
  HttpMethod::PUT,
  HttpMethod::PATCH,
  HttpMethod::DELETE
};

Utils::Utils()
{
}

Utils::~Utils()
{
}

QString Utils::toString(HttpMethod method)
{
  switch(method)
  {
    case HttpMethod::GET:
      return "GET";

    case HttpMethod::POST:
      return "POST";

    case HttpMethod::PUT:
      return "PUT";

    case HttpMethod::DELETE:
      return "DELETE";

    case HttpMethod::PATCH:
      return "PATCH";

    default:
      return "";
  }
}

HttpMethod Utils::fromString(const QString& method)
{
  QString str = method.trimmed().toUpper();
  if(str == "GET")
  {
    return HttpMethod::GET;
  }
  if(str == "POST")
  {
    return HttpMethod::POST;
  }
  if(str == "PUT")
  {
    return HttpMethod::PUT;
  }
  if(str == "DELETE")
  {
    return HttpMethod::DELETE;
  }
  if(str == "PATCH")
  {
    return HttpMethod::PATCH;
  }
  throw QttpException("Unrecognized http method " + method.toStdString());
}

HttpMethod Utils::fromStdString(const std::string& method)
{
  return fromString(QString::fromStdString(method));
}

QJsonObject Utils::readJson(const QString& path)
{
  QFile file(path);
  if(!file.exists())
  {
    LOG_DEBUG("File does not exist [" + path + "]");
    return QJsonObject();
  }
  if(!file.open(QIODevice::ReadOnly))
  {
    LOG_DEBUG("Unable to open [" + path + "]");
    return QJsonObject();
  }

  QJsonParseError parseError;
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);

  if(parseError.error != QJsonParseError::NoError)
  {
    THROW_EXCEPTION((path + ": " + parseError.errorString()).toStdString());
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
  m_File(QDir::temp().absoluteFilePath(QString("qttp-").
                                       append(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss-")).
                                       append(std::to_string(QCoreApplication::applicationPid()).c_str()).
                                       append(".log"))),
  m_Stream(&m_File),
  m_OriginalMessageHandler(nullptr),
  m_TimerId(-1)
{
}

LoggingUtils::~LoggingUtils()
{
  if(m_TimerId > -1) this->killTimer(m_TimerId);
  if(m_File.isOpen()) m_File.close();
}

bool LoggingUtils::initializeFile(const QString& filename, quint32 writeFrequency)
{
  if(!filename.isEmpty())
  {
    m_File.setFileName(filename);
  }

  if(m_File.isOpen())
  {
    LOG_WARN("Log file is already open");
    return false;
  }

  if(m_File.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    LOG_DEBUG("Using log file" << m_File.fileName());

    if(m_OriginalMessageHandler == nullptr)
    {
      m_OriginalMessageHandler = qInstallMessageHandler(LoggingUtils::fileLogger);
    }

    // If our flush duration is 0 we intend to always flush on every write so
    // we'll check against m_TimerId's default value of -1.
    if(writeFrequency > 0)
    {
      LOG_DEBUG("Flushing to log file every" << writeFrequency << "ms");
      m_TimerId = this->startTimer(writeFrequency, Qt::CoarseTimer);
    }
    else
    {
      LOG_DEBUG("Flush frequency is not set, will flush on every single write");
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

  if(log.m_OriginalMessageHandler == nullptr)
  {
    return;
  }

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
    log.m_Stream << msg << '\r\n';
  #else
    log.m_Stream << msg << '\n';
  #endif
  }
}
