#ifndef QTTPACTION_H
#define QTTPACTION_H

#include "qttp_global.h"
#include "httpdata.h"

namespace qttp
{

class Action;

class QTTPSHARED_EXPORT Input
{
  public:
    Input();
    Input(const QString& name);
    Input(const QString& name, const QString& desc);
    Input(const QString& name, const QStringList& enums);
    Input(const QString& name, const QStringList& enums, const QString& desc);

    QString name;
    QString description;
    bool required;

    //! Should match "in"
    QString paramType;

    //! Should match "type"
    QString dataType;

    //! Should match "enum"
    QStringList enums;
};

class QTTPSHARED_EXPORT RequiredInput : public Input
{
  public:
    RequiredInput();
    RequiredInput(const QString& name);
    RequiredInput(const QString& name, const QString& desc);
    RequiredInput(const QString& name, const QStringList& enums);
    RequiredInput(const QString& name, const QStringList& enums, const QString& desc);
};

class HttpServer;

/**
 * @brief Borrowing ideas from Node.js frameworks like Actionhero.js, an action
 * is ultimately the endpoint of the http request.
 */
class QTTPSHARED_EXPORT Action
{
  friend class HttpServer;

  public:
    Action();
    virtual ~Action();

    /**
     * @brief Primary entry point for processing actions. Default implementation
     * will invoke corresponding members against the HTTP methods: onGet(),
     * onPost(), onPut(), and onDelete()
     * @param data
     */
    virtual void onAction(HttpData& data);
    virtual void onGet(HttpData& data);
    virtual void onPost(HttpData& data);
    virtual void onPut(HttpData& data);
    virtual void onPatch(HttpData& data);
    virtual void onHead(HttpData& data);
    virtual void onDelete(HttpData& data);
    virtual void onOptions(HttpData& data);
    virtual void onTrace(HttpData& data);
    virtual void onConnect(HttpData& data);
    virtual void onUnknown(HttpData& data);

    /**
     * @brief Override  in order to associate this action to a specific
     * HttpMethod and route (e.g. "/myroute/").
     */
    virtual const QList<std::pair<HttpMethod, QString> >& getRoutes() const;

    virtual const char* getName() const = 0;
    virtual const char* getSummary() const;
    virtual const char* getDescription() const;
    virtual const QStringList& getTags() const;
    virtual const QList<Input>& getInputs() const;

  protected:

    /**
     * @brief Override in order to return a list of headers to automatically
     * append to each response.
     */
    virtual const QList<std::pair<std::string, std::string> >& getHeaders() const;

    /**
     * @brief Helps apply, modify, or prune headers in each response.
     */
    virtual void applyHeaders(HttpData& data) const;

  private:
    static const QList<Input> m_EmptyInputList;
    static const QStringList m_EmptyStringList;
    static const QList<std::pair<std::string, std::string> > m_EmptyStringPairList;
};

/**
 * @brief Like an interceptor, filter or a pipeline design pattern, this
 * processor is available for frequent and commonly performed tasks.  Again,
 * borrows from the node.js misnomer - known as a "middleware".
 */
class QTTPSHARED_EXPORT Processor
{
  public:
    Processor();
    virtual ~Processor();

    virtual const char* getName() const = 0;

    /// @brief Invoked before qttp::Action::onAction().
    virtual void preprocess(HttpData& data);

    /// @brief Invoked after qttp::Action::onAction().
    virtual void postprocess(HttpData& data);
};

}

#endif // QTTPACTION_H
