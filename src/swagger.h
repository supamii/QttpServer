#ifndef SWAGGER_H
#define SWAGGER_H

#include "action.h"

namespace qttp
{

/**
 * @brief When included, this should help reveal all public API end points in
 * Swagger-consumable JSON format.
 *
 */
class QTTPSHARED_EXPORT Swagger : public Action
{
  public:

    Swagger();
    ~Swagger();

    void initialize();

    const char* getName() const;
    std::set<HttpPath> getRoutes() const;

    static const char* ACTION_NAME;

QTTP_PRIVATE:

    /**
     * @brief Returns swagger-ready json for consumption
     * @param data
     */
    void onGet(HttpData& data);

    bool m_IsEnabled;
    QJsonObject m_Response;
};

}
#endif // SWAGGER_H
