#ifndef QTTPHTTPRESPONSE_H
#define QTTPHTTPRESPONSE_H

#include "qttp_global.h"
#include "httpurl.h"

namespace qttp
{

class HttpServer;
class HttpData;

class QTTPSHARED_EXPORT HttpResponse
{
    friend class HttpServer;
    friend class HttpData;

  private:
    HttpResponse(native::http::response* resp);
    ~HttpResponse();

  public:

    bool end(const std::string& body);
    bool end(int length, const char* body);
    void write(const std::string& body);
    void write(int length, const char* body);
    bool close();

    void set_status(int status_code);
    int get_status() const;

    void set_header(const std::string& key, const std::string& value);
    const std::map<std::string, std::string, native::text::ci_less>& get_headers() const;

    bool getsockname(bool& ip4, std::string& ip, int& port);
    bool getpeername(bool& ip4, std::string& ip, int& port);

    static std::string get_status_text(int status);

  private:
    Assert<native::http::response> m_Assertion;
    native::http::response* m_Response;
};

}

#endif // QTTPHTTPRESPONSE_H
