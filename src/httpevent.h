#ifndef QTTPHTTPEVENT_H
#define QTTPHTTPEVENT_H

#include <QObject>
#include <QEvent>
#include <QDateTime>
#include <http.h>

namespace qttp
{

class HttpEvent : public QEvent
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
