#ifndef HTTPEVENT_H
#define HTTPEVENT_H

#include <QObject>
#include <QEvent>
#include <http.h>

namespace qttp
{

class HttpEvent : public QEvent
{
  public:
    HttpEvent();
    HttpEvent(std::pair<native::http::request*, native::http::response*>);
    HttpEvent(native::http::request*, native::http::response*);
    virtual ~HttpEvent();

    void setData(const std::pair<native::http::request*, native::http::response*>&);
    std::pair<native::http::request*, native::http::response*>& getData();
    std::pair<native::http::request*, native::http::response*> getData() const;

  private:
    std::pair<native::http::request*, native::http::response*> m_Data;
};

} // End namespace qttp

#endif // HTTPEVENT_H
