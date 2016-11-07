#ifndef QTTPHTTPEVENT_H
#define QTTPHTTPEVENT_H

#include "qttp_global.h"

namespace qttp
{

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

class QTTPSHARED_EXPORT NativeHttpEvent : public QEvent
{
  public:

    NativeHttpEvent();
    NativeHttpEvent(native::http::request*, native::http::response*);
    virtual ~NativeHttpEvent();

    native::http::request* getRequest() const;
    native::http::response* getResponse() const;
    const QDateTime& getTimestamp() const;

QTTP_PRIVATE:

    native::http::request * m_Request;
    native::http::response* m_Response;
    QDateTime m_Timestamp;
};


} // End namespace qttp

#endif // QTTPHTTPEVENT_H
