#ifndef HTTPEVENTHANDLER_H
#define HTTPEVENTHANDLER_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>

class HttpEventHandler : public QObject
{
    Q_OBJECT

public:
    HttpEventHandler();
    static HttpEventHandler* getInstance();
    ~HttpEventHandler();
    bool eventFilter(QObject *object, QEvent *event);

private:
    static std::unique_ptr<HttpEventHandler> m_Instance;
};

#endif // HTTPEVENTHANDLER_H
