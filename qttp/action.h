#ifndef QTTPACTION_H
#define QTTPACTION_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>
#include <functional>

#include "httpdata.h"

namespace qttp
{

class Action
{
  public:
    Action();
    virtual ~Action();
    virtual void onAction(HttpData& data) = 0;
    virtual std::string getActionName() = 0;
};

}

#endif // QTTPACTION_H
