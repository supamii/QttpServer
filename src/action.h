#ifndef QTTPACTION_H
#define QTTPACTION_H

#include "qttp_global.h"
#include "httproute.h"
#include "httpdata.h"

namespace qttp
{

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
     * HttpMethod and path (e.g. "/myroute/").
     */
    virtual QList<qttp::HttpPath> getRoutes() const;

    virtual const char* getName() const = 0;
    virtual const char* getSummary() const;
    virtual const char* getDescription() const;
    virtual QStringList getTags() const;
    virtual std::vector<Input> getInputs() const;

  protected:

    /**
     * @brief Override in order to return a list of headers to automatically
     * append to each response.  This returns a reference to a vector because
     * this can be called frequently when assembling the outbound response.
     */
    virtual std::vector<QStringPair> getHeaders() const;

    /**
     * @brief Helps apply, modify, or prune headers in each response.
     */
    virtual void applyHeaders(HttpData& data) const;

  private:

    static const QList<qttp::HttpPath> m_EmptyRoutes;
    static const std::vector<Input> m_EmptyInputList;
    static const QStringList m_EmptyStringList;
    static const std::vector<QStringPair> m_EmptyStringPairList;
};

/**
 * @brief A Simple action that stores a basic call-back.  This will allow
 * us to supplement with additional information.
 */
class QTTPSHARED_EXPORT SimpleAction : public Action
{
  friend class HttpServer;

  public:

    SimpleAction(std::function<void(qttp::HttpData&)> callback);
    virtual ~SimpleAction();

    void onAction(HttpData& data);

    const char* getName() const;

    void setRoutes(const QList<qttp::HttpPath>& routes);
    QList<qttp::HttpPath> getRoutes() const;

    void setSummary(const char* summary);
    const char* getSummary() const;

    void setDescription(const char* description);
    const char* getDescription() const;

    void setTags(const QStringList& tags);
    QStringList getTags() const;

    void setInputs(const std::vector<Input>& inputs);
    std::vector<Input> getInputs() const;

  protected:
    std::vector<QStringPair> getHeaders() const;

  private:
    std::function<void(qttp::HttpData&)> m_Callback;
    QByteArray m_Name;
    QList<qttp::HttpPath> m_Routes;
    QByteArray m_Summary;
    QByteArray m_Description;
    QStringList m_Tags;
    std::vector<Input> m_Inputs;
    std::vector<QStringPair> m_Headers;
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

} // End namespace qttp

#endif // QTTPACTION_H
