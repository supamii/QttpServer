#ifndef __NATIVE_QHTTP_H__
#define __NATIVE_QHTTP_H__

#include <stdio.h>
#include <sys/types.h>
#include <sstream>
#include <iostream>
#include <http_parser.h>
#include "base.h"
#include "handle.h"
#include "net.h"
#include "tcp.h"
#include "text.h"
#include "callback.h"
#include "http.h"

#include <QtCore>

namespace native
{
namespace http
{

class QttpUrl
{
  friend class QttpClientContext;

  public:
    QttpUrl();
    ~QttpUrl();

    QttpUrl(const QttpUrl& c);
    QttpUrl& operator =(const QttpUrl& c);

  public:
    QByteArray schema() const;
    QByteArray host() const;
    int port() const;
    QByteArray path() const;
    QByteArray query() const;
    QByteArray fragment() const;

  private:
    void from_buf(const char* buf, std::size_t len, bool is_connect = false);

    bool has_schema() const {
      return (handle_.field_set & (1 << UF_SCHEMA)) != 0;
    }
    bool has_host() const {
      return (handle_.field_set & (1 << UF_HOST)) != 0;
    }
    bool has_port() const {
      return (handle_.field_set & (1 << UF_PORT)) != 0;
    }
    bool has_path() const {
      return (handle_.field_set & (1 << UF_PATH)) != 0;
    }
    bool has_query() const {
      return (handle_.field_set & (1 << UF_QUERY)) != 0;
    }
    bool has_fragment() const {
      return (handle_.field_set & (1 << UF_FRAGMENT)) != 0;
    }

  private:
    http_parser_url handle_;
    QByteArray buf_;
};

class QttpClientContext;
typedef std::shared_ptr<QttpClientContext> qttp_client_ptr;

class QttpResponse
{
  friend class QttpClientContext;

  private:
    QttpResponse(QttpClientContext* client, native::net::tcp* socket);
    ~QttpResponse();

  public:

    bool end(const QString& body) {
      write(body.length(), body.constData());
      return close();
    }

    void write(int length, const QChar* body);

    bool end(const QByteArray& body) {
      write(body.length(), body.constData());
      return close();
    }

    bool end(const std::string& body) {
      return end(body.length(), body.c_str());
    }

    bool end(size_t length, const char* body)
    {
      write(length, body);
      return close();
    }

    void write(const std::string& body)
    {
      return write(body.length(), body.c_str());
    }

    void write(size_t length, const char* body);

    bool close();

    void set_status(int status_code) {
      status_ = status_code;
    }

    int get_status() const {
      return status_;
    }

    void set_header(const QString& key, const QString& value) {
      headers_[key] = value;
    }

    const std::map<QString, QString>& get_headers() const {
      return headers_;
    }

    bool getsockname(bool& ip4, std::string& ip, int& port) const
    {
      return socket_->getsockname(ip4, ip, port);
    }

    bool getpeername(bool& ip4, std::string& ip, int& port) const
    {
      return socket_->getpeername(ip4, ip, port);
    }

  private:
    qttp_client_ptr client_;
    native::net::tcp* socket_;
    std::map<QString, QString> headers_;
    int status_;
    QByteArray response_data_;
    bool is_response_written_;
};

class QttpRequest
{
  friend class QttpClientContext;

  private:
    QttpRequest();
    ~QttpRequest();

  public:
    const QttpUrl& url() const {
      return url_;
    }

    const QString& get_header(const QString& key) const;
    bool get_header(const QString& key, QString& value) const;
    const std::map<QString, QString>& get_headers() const;

    const QByteArray& get_body (void) const {
      return body_;
    }

    const QString& get_method (void) const {
      return method_;
    }

    uint64_t get_timestamp(void) const {
      return timestamp_;
    }

  private:
    QttpUrl url_;
    std::map<QString, QString> headers_;
    QByteArray body_;
    QString method_;
    uint64_t timestamp_;

    static const QString default_value_;
};

class QttpClientContext
{
  friend class Qttp;

  private:
    QttpClientContext(native::net::tcp* server);

  public:
    ~QttpClientContext();

  private:
    bool parse(std::function<void(QttpRequest&, QttpResponse&)> callback);

  private:
    http_parser parser_;
    http_parser_settings parser_settings_;
    bool was_header_value_;
    QString last_header_field_;
    QString last_header_value_;

    std::shared_ptr<native::net::tcp> socket_;
    QttpRequest* request_;
    QttpResponse* response_;

    callbacks* callback_lut_;
};

class Qttp
{
  public:
    Qttp();
    virtual ~Qttp();

  public:
    bool listen(const std::string& ip, int port, std::function<void(QttpRequest&, QttpResponse&)> callback);

  private:
    std::shared_ptr<native::net::tcp> socket_;
};

}
}

#endif // __NATIVE_QHTTP_H__
