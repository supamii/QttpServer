#ifndef HTTPEVENT_H
#define HTTPEVENT_H

#include <QObject>
#include <QEvent>
#include <http.h>

class HttpEvent : public QEvent
{
public:
    HttpEvent();
    HttpEvent(std::pair<native::http::request*, native::http::response*>);
    HttpEvent(native::http::request*, native::http::response*);
    ~HttpEvent();

    void setData(const std::pair<native::http::request*, native::http::response*>&);
    std::pair<native::http::request*, native::http::response*>& getData();
    std::pair<native::http::request*, native::http::response*> getData() const;

private:
    std::pair<native::http::request*, native::http::response*> data;
};

#endif // HTTPEVENT_H
