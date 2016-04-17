#ifndef QTTPHTTPEVENT_H
#define QTTPHTTPEVENT_H

#include "qttp_global.h"

namespace qttp
{

class QTTPSHARED_EXPORT HttpEvent : public QEvent
{
  public:
    HttpEvent();
    HttpEvent(native::http::request*, native::http::response*);
    virtual ~HttpEvent();

    native::http::request* getRequest() const;
    native::http::response* getResponse() const;
    const QDateTime& getTimestamp() const;

  private:
    native::http::request* m_Request;
    native::http::response* m_Response;
    QDateTime m_Timestamp;
};

} // End namespace qttp

#endif // QTTPHTTPEVENT_H
