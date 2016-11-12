#ifndef QTTPHTTPEVENT_H
#define QTTPHTTPEVENT_H

#include "qttp_global.h"

namespace qttp
{

/**
 * @brief This class is generally for internal use only.  It derives from
 * QEvent in order forward data from libuv's eventloop to Qt's eventloop.
 */
class QTTPSHARED_EXPORT HttpEvent : public QEvent
{
  public:

    HttpEvent();
    HttpEvent(native::http::QttpRequest*, native::http::QttpResponse*);
    virtual ~HttpEvent();

    native::http::QttpRequest* getRequest() const;
    native::http::QttpResponse* getResponse() const;
    const QDateTime& getTimestamp() const;

QTTP_PRIVATE:

    native::http::QttpRequest * m_Request;
    native::http::QttpResponse* m_Response;
    QDateTime m_Timestamp;
};

} // End namespace qttp

#endif // QTTPHTTPEVENT_H
