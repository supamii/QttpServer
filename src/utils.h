#ifndef QTTPUTILS_H
#define QTTPUTILS_H

#include "qttp_global.h"

#ifndef THROW_RUNTIME
#define THROW_RUNTIME(X) std::stringstream x; x << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << X; throw std::runtime_error(x.str().c_str())
#endif

#ifndef THROW_STD_EXCEPTION
#define THROW_STD_EXCEPTION(X) std::stringstream x; x << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << X; throw qttp::QttpException(x.str())
#endif

#ifndef THROW_EXCEPTION
#define THROW_EXCEPTION(X) QString s; QTextStream x(&s); x << __FILE__ << ":" << __FUNCTION__ << ":" << __LINE__ << ": " << X; throw qttp::QttpException(s.toStdString());
#endif

#ifdef QTTP_DISABLE_LOGGING
  #define LOG_DATETIME QString()
  #define LOG_FILE QString()
  #define LOG_FUNCTION(X)
  #define LOG_TRACE
  #define LOG_DBG(X)
  #define LOG_DEBUG(X)
  #define LOG_INFO(X)
  #define LOG_WARN(X)
  #define LOG_ALERT(X)
  #define LOG_ERROR(X)
  #define LOG_FATAL(X)
#else
  #ifdef __ECLIPSE_WORKAROUND__
    #ifndef __FUNCSIG__
      #define __FUNCSIG__ ""
    #endif
  #endif

#ifdef QTTP_OMIT_LOG_DATETIME
  #define LOG_DATETIME QString()
#else
// TODO If we're running as a unit test - ignore time stamps!?
  #ifndef LOG_DATETIME
    #define LOG_DATETIME QDateTime::currentDateTime().toString("yyyy/MM/dd-hh:mm:ss:zzz").append(' ')
  #endif
#endif

// A bit nasty and expensive string creation - strictly for debugging so
// in release mode the build should define NO_LOG_FILE or NO_LOG_FUNCTION
  #ifndef LOG_FILE
// TODO This doesn't check cygwin but we also need to address the fact that
// MSVC tends to attach the demangled namespace to the __FUNCTION__ macro
// anyway
    #ifdef Q_OS_WIN
      #define LOG_FILE QString(__FILE__).mid(QString(__FILE__).lastIndexOf('\\') + 1).append(":")
    #else
      #define LOG_FILE QString(__FILE__).mid(QString(__FILE__).lastIndexOf('/') + 1).append(":")
    #endif
  #endif

  #ifdef NO_LOG_FILE
    #define LOG_FILE QString()
  #endif

  #ifndef LOG_FUNCTION
// Sample below confuses QtCreator syntax editor:
// QString(__FUNCTION__":%1").arg(__LINE__)
    #define LOG_FUNCTION(LEVEL) LOG_DATETIME.append(LEVEL) << LOG_FILE.append(__FUNCTION__).append(":").append(QString::number(__LINE__))
  #endif

// If the function and line numbers are too noisy then define
// NO_LOG_FUNCTION to remove it from print statements
  #ifdef NO_LOG_FUNCTION
    #define LOG_FUNCTION(LEVEL) LOG_DATETIME.append(LEVEL)
  #endif

  #ifndef LOG_TRACE
    #define LOG_TRACE qttp::LogTrace logTraceObject(LOG_FILE.append(__FUNCTION__), __LINE__)
  #endif

// The no-quote feature may not be available on all version of Qt so this
// definition should help stub it out.

  #ifdef QDEBUG_NOQUOTE_NOT_AVAILABLE
  #  define QDEBUG_NOQUOTE qDebug()
  #  define QINFO_NOQUOTE qInfo()
  #  define QWARNING_NOQUOTE qWarning()
  #  define QCRITICAL_NOQUOTE qCritical()
  #else
  #  define QDEBUG_NOQUOTE qDebug().noquote()
  #  define QINFO_NOQUOTE qInfo().noquote()
  #  define QWARNING_NOQUOTE qWarning().noquote()
  #  define QCRITICAL_NOQUOTE qCritical().noquote()
  #endif

  #ifndef LOG_DBG
    #define LOG_DBG(X) QDEBUG_NOQUOTE << LOG_FUNCTION("DEBUG") << X
  #endif

  #ifndef LOG_DEBUG
    #define LOG_DEBUG(X) LOG_DBG(X)
  #endif

  #ifndef LOG_INFO
    #define LOG_INFO(X) QINFO_NOQUOTE << LOG_FUNCTION("INFO ") << X
  #endif

  #ifndef LOG_WARN
    #define LOG_WARN(X) QWARNING_NOQUOTE << LOG_FUNCTION("WARN ") << X
  #endif

  #ifndef LOG_ALERT
    #define LOG_ALERT(X) QCRITICAL_NOQUOTE << LOG_FUNCTION("ALERT") << X
  #endif

  #ifndef LOG_ERROR
    #define LOG_ERROR(X) QCRITICAL_NOQUOTE << LOG_FUNCTION("ERROR") << X
  #endif

  #ifndef LOG_FATAL
    #define LOG_FATAL(X) LOG_ERROR(X); { QByteArray fatal; QTextStream fts(&fatal); fts << "FATAL" << __FUNCTION__ << ":" << (int)__LINE__ << " " << X; fatal.append('\0'); qFatal("%s", fatal.constData()); }
  #endif
#endif

namespace qttp
{

// Forward declaration
class HttpServer;

/**
 * @brief This takes from ideas in ACE C++ to print on construction and
 * destruction of the object for easier log-tracing.
 */
class QTTPSHARED_EXPORT LogTrace
{
  public:
    LogTrace(QString, quint32);
    ~LogTrace();
    QString function;
    quint32 line;
};

#ifdef _MSC_VER
#define QTTP_NOEXCEPT _NOEXCEPT
#else
#define QTTP_NOEXCEPT noexcept
#endif

class QTTPSHARED_EXPORT QttpException : public std::exception
{
  public:
    QttpException(const std::string& message);
    const char* what() const QTTP_NOEXCEPT;

QTTP_PRIVATE:

    std::string m_Message;
};

class QTTPSHARED_EXPORT Utils
{
  Utils();

  public:

    static const char* toString(HttpMethod method);

    static const char* toStringLower(HttpMethod method);

    template<class T> static HttpMethod fromString(const T& method)
    {
      if(method == "GET")
      {
        return HttpMethod::GET;
      }
      if(method == "POST")
      {
        return HttpMethod::POST;
      }
      if(method == "PUT")
      {
        return HttpMethod::PUT;
      }
      if(method == "PATCH")
      {
        return HttpMethod::PATCH;
      }
      if(method == "HEAD")
      {
        return HttpMethod::HEAD;
      }
      if(method == "OPTIONS")
      {
        return HttpMethod::OPTIONS;
      }
      if(method == "DELETE")
      {
        return HttpMethod::DEL;
      }
      if(method == "TRACE")
      {
        return HttpMethod::TRACE;
      }
      if(method == "CONNECT")
      {
        return HttpMethod::CONNECT;
      }

#ifdef QTTP_PRODUCTION_MODE
      return HttpMethod::UNKNOWN;
#else
      // Make sure we throw to fail fast in dev mode.
      THROW_EXCEPTION("Unrecognized http method [" << method << "]");
#endif
    }

    template<class T> static HttpMethod fromPartialString(const T& str)
    {
      auto firstChar = str[0];

      if(firstChar == 'G'|| firstChar == 'g')
      {
        return HttpMethod::GET;
      }

      if(firstChar == 'P'|| firstChar == 'p')
      {
        auto secondChar = str[1];

        if(secondChar == 'U'|| secondChar == 'u')
        {
          return HttpMethod::PUT;
        }
        if(secondChar == 'O'|| secondChar == 'o')
        {
          return HttpMethod::POST;
        }
        if(secondChar == 'A'|| secondChar == 'a')
        {
          return HttpMethod::PATCH;
        }
      }

      if(firstChar == 'H'|| firstChar == 'h')
      {
        return HttpMethod::HEAD;
      }

      if(firstChar == 'O' || firstChar == 'o')
      {
        return HttpMethod::OPTIONS;
      }

      if(firstChar == 'D'|| firstChar == 'd')
      {
        return HttpMethod::DEL;
      }

      if(firstChar == 'T' || firstChar == 't')
      {
        return HttpMethod::TRACE;
      }

      if(firstChar == 'C' || firstChar == 'c')
      {
        return HttpMethod::CONNECT;
      }

#ifdef QTTP_PRODUCTION_MODE
      return HttpMethod::UNKNOWN;
#else
      // Make sure we throw to fail fast in dev mode.
      THROW_EXCEPTION("Unrecognized http method");
#endif
    }

    /**
     * @brief Throws a std::runtime_error with a message of any JSON validation
     * errors that were encountered.
     *
     * @param path
     * @return
     */
    static QJsonObject readJson(const QString& path);

    static inline QByteArray toByteArray(const std::string& str)
    {
      return QByteArray(str.c_str(), str.length());
    }

    static inline QByteArray toByteArray(const std::stringstream& buffer)
    {
      return Utils::toByteArray(buffer.str());
    }

    static inline QJsonObject toJson(const std::stringstream& buffer, QJsonParseError* error = 0)
    {
      return QJsonDocument::fromJson(toByteArray(buffer), error).object();
    }

    static inline QJsonObject toJson(const std::string& str, QJsonParseError* error = 0)
    {
      return QJsonDocument::fromJson(toByteArray(str), error).object();
    }

    static inline QJsonObject toJson(QByteArray bytes, QJsonParseError* error = 0)
    {
      return QJsonDocument::fromJson(bytes, error).object();
    }

    static inline QJsonArray toArray(const std::stringstream& buffer, QJsonParseError* error = 0)
    {
      return QJsonDocument::fromJson(toByteArray(buffer), error).array();
    }

    static inline QJsonArray toArray(const std::string& str, QJsonParseError* error = 0)
    {
      return QJsonDocument::fromJson(toByteArray(str), error).array();
    }

    static inline QJsonArray toArray(QByteArray bytes, QJsonParseError* error = 0)
    {
      return QJsonDocument::fromJson(bytes, error).array();
    }
};

class QTTPSHARED_EXPORT Stats
{
  friend class HttpServer;

  public:

    Stats();
    ~Stats();
    void increment(const QString& key);
    void setValue(const QString& key, const QVariant& value);

QTTP_PRIVATE:

    QHash<QString, QVariant> m_Statistics;
};

class QTTPSHARED_EXPORT LoggingUtils : public QObject
{
  Q_OBJECT

  public:
    LoggingUtils();
    ~LoggingUtils();

    static void fileLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    bool initializeFile(const QString& filename = QString(), quint32 writeFrequency = 300);

    bool initializeSysLog();

    QMutex& getMutex()
    {
      return m_Mutex;
    }

    const QFile& getFile() const
    {
      return m_File;
    }

    QtMessageHandler getOriginalMessageHandler()
    {
      return m_OriginalMessageHandler;
    }

    void timerEvent(QTimerEvent* event);

QTTP_PRIVATE:

    QMutex m_Mutex;
    QFile m_File;
    QTextStream m_Stream;
    QtMessageHandler m_OriginalMessageHandler;
    qint32 m_TimerId;
};

} // End namespace qttp

#endif // QTTPUTILS_H
