#ifndef QTTPHTTPEVENT_H
#define QTTPHTTPEVENT_H

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

    void setCmd(const QString& cmd);
    const QString& getCmd() const;

  private:
    std::pair<native::http::request*, native::http::response*> m_Data;
    QString m_Cmd;
};

} // End namespace qttp

#endif // QTTPHTTPEVENT_H
