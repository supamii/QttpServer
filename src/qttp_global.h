#ifndef QTTP_GLOBAL_H
#define QTTP_GLOBAL_H

#ifdef QTTP_LIBRARY
#  if defined(QTTP_EXPORT)
#    define QTTPSHARED_EXPORT Q_DECL_EXPORT
#  else
#    define QTTPSHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QTTPSHARED_EXPORT
#endif

#ifndef QTTP_SERVER_VERSION
#  define QTTP_SERVER_VERSION "QttpServer/1.0.0"
#endif

#include <QtCore>
#include <QtNetwork>
#include <QObject>
#include <QFile>
#include <QJsonDocument>
#include <QEvent>
#include <QDateTime>
#include <QProcessEnvironment>
#include <QJsonObject>
#include <QList>

#include <http.h>
#include <functional>

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <unordered_map>
#include <thread>
#include <initializer_list>

// Unfortunately, some compilers will default to bool/int before it assumes
// QString.  For these cases we'll resort to this ugly MACRO.

#ifndef QSTR
#  ifdef USE_QSTRING_LITERAL
#    define QSTR(X) QStringLiteral(X)
#  else
#    define QSTR(X) X
#  endif
#endif

namespace qttp
{

typedef enum DataControl
{
  // Provided for Processors to abort/terminate processing.
  Terminated = 0x0001,
  // Set internally to indicate that the response was sent.
  Finished = 0x0002,
  // Set internally to indicate that preprocessors operated on this.
  Preprocessed = 0x0004,
  // Set internally to indicate that postprocessors operated on this.
  Postprocessed = 0x0008,
  // Set internally to indicate this was operated on by an Action.
  ActionProcessed = 0x0010,
  // Default value.
  None = 0x0000

} DataControl;

enum class HttpMethod : char
{
  UNKNOWN = -1,
  GET = 0,
  POST = 1,
  PUT = 2,
  DEL = 3,
  PATCH = 4,
  HEAD = 5,
  OPTIONS = 6,
  CONNECT = 7,
  TRACE = 8,
};

/**
 * Extends int in order to match the internal type inside native::http:response.
 *
 * Should consider moving this into native::http so we don't fall out of sync.
 */
enum class HttpStatus : int
{
  // "Continue";
  CONTINUE = 100,
  // "Switching Protocols";
  SWITCHING_PROTOCOLS  = 101,
  // "Processing"; // RFC 2518, obsoleted by RFC 4918
  PROCESSING = 102,
  // "OK";
  OK = 200,
  // "Created";
  CREATED = 201,
  // "Accepted";
  ACCEPTED  = 202,
  // "Non-Authoritative Information";
  NON_AUTHORITATIVE_INFORMATION  = 203,
  // "No Content";
  NO_CONTENT  = 204,
  // "Reset Content";
  RESET_CONTENT  = 205,
  // "Partial Content";
  PARTIAL_CONTENT = 206,
  // "Multi-Status";
  MULTI_STATUS = 207,
  // "Multiple Choices";
  MULTIPLE_CHOICES = 300,
  // "Moved Permanently";
  MOVED_PERMANENTLY = 301,
  // "Found";
  FOUND = 302,
  // "See Other";
  SEE_OTHER = 303,
  // "Not Modified";
  NOT_MODIFIED = 304,
  // "Use Proxy";
  USE_PROXY = 305,
  // RESERVED = 306, // "(reserved)";
  // "Temporary Redirect";
  TEMPORARY_REDIRECT  = 307,
  // "Permanent Redirect"; // RFC 7238
  PERMANENT_REDIRECT = 308,
  // "Bad Request";
  BAD_REQUEST = 400,
  // "Unauthorized";
  UNAUTHORIZED = 401,
  // "Payment Required";
  PAYMENT_REQUIRED = 402,
  // "Forbidden";
  FORBIDDEN = 403,
  // "Not Found";
  NOT_FOUND = 404,
  // "Method Not Allowed";
  METHOD_NOT_ALLOWED = 405,
  // "Not Acceptable";
  NOT_ACCEPTABLE = 406,
  // "Proxy Authentication Required";
  PROXY_AUTHENTICATION_REQUIRED = 407,
  // "Request Timeout";
  REQUEST_TIMEOUT = 408,
  // "Conflict";
  CONFLICT = 409,
  // "Gone";
  GONE = 410,
  // "Length Required";
  LENGTH_REQUIRED = 411,
  // "Precondition Failed";
  PRECONDITION_FAILED = 412,
  // "Request Entity Too Large";
  REQUEST_ENTITY_TOO_LARGE = 413,
  // "Request-URI Too Long";
  REQUEST_URI_TOO_LONG = 414,
  // "Unsupported Media Type";
  UNSUPPORTED_MEDIA_TYPE = 415,
  // "Requested Range Not Satisfiable";
  REQUESTED_RANGE_NOT_SATISFIABLE = 416,
  // "Expectation Faiiled";
  EXPECTATION_FAILED = 417,
  // "I'm a teapot"; //RFC2324.
  IM_A_TEAPOT = 418,
  // "Unprocessable Entity"; // RFC 4918
  UNPROCESSABLE_ENTITY = 422,
  // "Locked"; // RFC 4918
  LOCKED = 423,
  // "Failed Dependency"; // RFC 4918
  FAILED_DEPENDENCY = 424,
  // "Unordered Collection"; // RFC 4918
  UNORDERED_COLLECTION = 425,
  // "Upgrade Required"; // RFC 2817
  UPGRADE_REQUIRED = 426,
  // "Precondition Required"; // RFC 6585
  PRECONDITION_REQUIRED = 428,
  // "Too Many Requests"; // RFC 6585
  TOO_MANY_REQUESTS = 429,
  // "Request Header Fields Too Large"; // RFC 6585
  REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
  // "Internal Server Error";
  INTERNAL_SERVER_ERROR = 500,
  // "Not Implemented";
  NOT_IMPLEMENTED = 501,
  // "Bad Gateway";
  BAD_GATEWAY = 502,
  // "Service Unavailable";
  SERVICE_UNAVAILABLE = 503,
  // "Gateway Time-out";
  GATEWAY_TIMEOUT = 504,
  // "HTTP Version Not Supported";
  HTTP_VERSION_NOT_SUPPORTED = 505,
  // "Variant Also Negotiates"; // RFC2295
  VARIANT_ALSO_NEGOTIATES = 506,
  // "Insufficient Storage"; // RFC4918
  INSUFFICIENT_STORAGE = 507,
  // "Bandwidth Limit Exceeded";
  BANDWIDTH_LIMIT_EXCEEDED = 509,
  // "Not Extended"; // RFC2774
  NOT_EXTENDED = 510,
  // "Network Authentication Required"; // RFC 6585
  NETWORK_AUTHENTICATION_REQUIRED = 511,
};

/// Quick a dirty mechanism to assert ptrs within the initializer list!
template<class T> class Assert
{
  public:
    Assert(T* ptr)
    {
      Q_ASSERT(ptr != nullptr);
    }
};

// Forward declaration.
class HttpServer;

class QTTPSHARED_EXPORT Global
{
  friend class HttpServer;
  Global();

  public:

    static const QList<HttpMethod> HTTP_METHODS;

    static const QList<std::pair<std::string, std::string> >& getDefaultHeaders();

  private:
    static QList<std::pair<std::string, std::string> > DEFAULT_HEADERS;
};

}

#include "utils.h"

#endif // QTTP_GLOBAL_H
