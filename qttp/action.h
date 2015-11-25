#ifndef QTTPACTION_H
#define QTTPACTION_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>
#include <functional>

namespace qttp
{

class Action
{
  public:
    Action();
    virtual ~Action();
    virtual void onAction(native::http::request*, native::http::response*) = 0;
    virtual std::string getActionName() = 0;
};

}

#endif // QTTPACTION_H
