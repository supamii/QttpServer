#ifndef QTTPACTION_H
#define QTTPACTION_H

#include <QtCore>
#include <QtNetwork>
#include <http.h>
#include <functional>

#include "../src/httpdata.h"

namespace qttp
{

/**
 * @brief Borrowing ideas from Node.js frameworks like Actionhero.js, an action
 * is ultimately the endpoint of the http request.
 */
class Action
{
  public:
    Action();
    virtual ~Action();
    /**
     * @brief Primary entry point for processing actions.
     * @param data
     */
    virtual void onAction(HttpData& data) = 0;
    virtual const std::string getActionName() const = 0;
};

/**
 * @brief Like an interceptor, filter or a pipeline design pattern, this
 * processor is available for frequent and commonly performed tasks.  Again,
 * borrows from the node.js misnomer - known as a "middleware".
 */
class Processor
{
  public:
    Processor();
    virtual ~Processor();

    virtual const std::string getProcessorName() const = 0;

    /// @brief Invoked before qttp::Action::onAction().
    virtual void preprocess(HttpData& data);

    /// @brief Invoked after qttp::Action::onAction().
    virtual void postprocess(HttpData& data);
};

}

#endif // QTTPACTION_H
