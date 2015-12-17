#ifndef QTTPUTILS_H
#define QTTPUTILS_H

#include <QtCore>
#include <QFile>
#include <QJsonDocument>

#ifdef NO_QTTP_LOGGING
  #define LOG_DATETIME
  #define LOG_FUNCTION(X)
  #define LOG_TRACE(X)
  #define LOG_DBG(X)
  #define LOG_INFO(X)
  #define LOG_WARN(X)
  #define LOG_ALERT(X)
  #define LOG_ERROR(X)
  #define LOG_FATAL(X)
#else
  // As the name implies, a lot of IDEs and editors are still struggling to
  // support C++1y and as well as platform nuances.
  #ifdef __ECLIPSE_WORKAROUND__
    #ifndef __FUNCSIG__
      #define __FUNCSIG__ ""
    #endif
  #endif

  // TODO If we're running as a unit test - ignore time stamps!?
  #ifndef LOG_DATETIME
    #define LOG_DATETIME QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss:sss")
  #endif

  #ifndef LOG_FUNCTION
    // If the function and line numbers are too noisy then define
    // NO_LOG_FUNCTION to remove it from print statements
    #ifdef NO_LOG_FUNCTION
      #define LOG_FUNCTION(LEVEL) LEVEL
    #else
      // Sample below confuses QtCreator syntax editor:
      // QString(__FUNCTION__":%1").arg(__LINE__)
      #define LOG_FUNCTION(LEVEL) LEVEL << QString(__FUNCTION__).append(":").append(std::to_string(__LINE__).c_str())
    #endif
  #endif

  #ifndef LOG_TRACE
    #define LOG_TRACE qttp::LogTrace logTraceObject(__FUNCTION__, __LINE__)
  #endif

  #ifndef LOG_DBG
    #define LOG_DBG(X) qDebug().noquote() << LOG_DATETIME << LOG_FUNCTION("DEBUG") << X
  #endif

  #ifndef LOG_INFO
    #define LOG_INFO(X) qInfo().noquote() << LOG_DATETIME << LOG_FUNCTION("INFO ") << X
  #endif

  #ifndef LOG_WARN
    #define LOG_WARN(X) qWarning().noquote() << LOG_DATETIME << LOG_FUNCTION("WARN ") << X
  #endif

  #ifndef LOG_ALERT
    #define LOG_ALERT(X) qCritical().noquote() << LOG_DATETIME << LOG_FUNCTION("ALERT") << X
  #endif

  #ifndef LOG_ERROR
    #define LOG_ERROR(X) qCritical().noquote() << LOG_DATETIME << LOG_FUNCTION("ERROR") << X
  #endif

  #ifndef LOG_FATAL
    #define LOG_FATAL(X) qFatal() << LOG_DATETIME << LOG_FUNCTION("FATAL") << X
  #endif
#endif

namespace qttp
{

/**
 * @brief This takes from ideas in ACE C++ to print on construction and
 * destruction of the object for easier log-tracing.
 */
class LogTrace
{
  public:
    LogTrace(QString, quint32);
    ~LogTrace();
    QString function;
    quint32 line;
};

class Utils
{
  public:
    Utils();
    virtual ~Utils();

    /**
     * @brief Throws a std::runtime_error with a message of any JSON validation
     * errors that were encountered.
     *
     * @param path
     * @return
     */
    static QJsonObject readJson(const QString& path);
};

} // End namespace qttp

#endif // QTTPUTILS_H
