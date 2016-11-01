#include "qttp.h"

using namespace native;
using namespace native::http;

QttpUrl::QttpUrl() :
  handle_(),
  buf_()
{
}

QttpUrl::QttpUrl(const QttpUrl& c) :
  handle_(c.handle_),
  buf_(c.buf_)
{
}

QttpUrl& QttpUrl::operator =(const QttpUrl& c)
{
  handle_ = c.handle_;
  buf_ = c.buf_;
  return *this;
}

QttpUrl::~QttpUrl()
{
}

QByteArray QttpUrl::schema() const
{
  if(has_schema()) return buf_.mid(handle_.field_data[UF_SCHEMA].off, handle_.field_data[UF_SCHEMA].len);
  return "HTTP";
}

QByteArray QttpUrl::host() const
{
  // TODO: if not specified, use host name
  if(has_schema()) return buf_.mid(handle_.field_data[UF_HOST].off, handle_.field_data[UF_HOST].len);
  return "localhost";
}

int QttpUrl::port() const
{
  if(has_path()) return static_cast<int>(handle_.port);
  return (schema() == "HTTP" ? 80 : 443);
}

QByteArray QttpUrl::path() const
{
  if(has_path()) return buf_.mid(handle_.field_data[UF_PATH].off, handle_.field_data[UF_PATH].len);
  return "/";
}

QByteArray QttpUrl::query() const
{
  if(has_query()) return buf_.mid(handle_.field_data[UF_QUERY].off, handle_.field_data[UF_QUERY].len);
  return "";
}

QByteArray QttpUrl::fragment() const
{
  if(has_query()) return buf_.mid(handle_.field_data[UF_FRAGMENT].off, handle_.field_data[UF_FRAGMENT].len);
  return "";
}

void QttpUrl::from_buf(const char* buf, std::size_t len, bool is_connect)
{
  // TODO: validate input parameters

  buf_.clear();
  buf_.append(buf, len);

  if(http_parser_parse_url(buf, len, is_connect, &handle_) != 0)
  {
    // failed for some reason
    // TODO: let the caller know the error code (or error message)
    throw url_parse_exception();
  }
}

QttpResponse::QttpResponse(QttpClientContext* client, native::net::tcp* socket) :
  client_(client),
  socket_(socket),
  headers_(),
  status_(200),
  response_data_(),
  is_response_written_(false)
{
  headers_["Content-Type"] = "text/html";
}

QttpResponse::~QttpResponse()
{
}

void QttpResponse::write(int length, const QChar* body)
{
  if(!is_response_written_)
  {
    is_response_written_ = true;
    if(headers_.find("Content-Length") == headers_.end())
    {
      headers_["Content-Length"] = QString::number((length > 0 && body != nullptr ? length : 0));
    }

    QTextStream stream(&response_data_);
    stream << "HTTP/1.1 " << status_ << " " << response::get_status_text(status_).c_str() << "\r\n";
    for(auto & h : headers_)
    {
      stream << h.first << ": " << h.second << "\r\n";
    }
    stream << "\r\n";
    stream.flush();

    if(length > 0 && body != nullptr)
    {
      response_data_.resize(response_data_.length() + length);
      for(int i = 0; i < length; ++i)
      {
        response_data_.append(body[i]);
      }
    }
  }
  else
  {
    if(length > 0 && body != nullptr)
    {
      response_data_.resize(response_data_.length() + length);
      for(int i = 0; i < length; ++i)
      {
        response_data_.append(body[i]);
      }
    }
  }
}

void QttpResponse::write(size_t length, const char* body)
{
  if(!is_response_written_)
  {
    is_response_written_ = true;
    if(headers_.find("Content-Length") == headers_.end())
    {
      headers_["Content-Length"] = QString::number((length > 0 && body != nullptr ? length : 0));
    }

    QTextStream stream(&response_data_);
    stream << "HTTP/1.1 " << status_ << " " << response::get_status_text(status_).c_str() << "\r\n";
    for(auto & h : headers_)
    {
      stream << h.first << ": " << h.second << "\r\n";
    }
    stream << "\r\n";
    stream.flush();

    if(length > 0 && body != nullptr)
    {
      response_data_.append(body, length);
    }
  }
  else
  {
    if(length > 0 && body != nullptr)
    {
      response_data_.append(body, length);
    }
  }
}

bool QttpResponse::close()
{
  auto str = response_data_.constData();
  PRINT_DBG(str);
  return socket_->write(str, static_cast<int>(response_data_.length()), [ = ](native::error e) {
    if(e)
    {
      PRINT_STDERR("ERROR while trying to close QttpResponse");
      PRINT_NN_ERROR(e);
    }
    // clean up
    client_.reset();
  });
}

const QString QttpRequest::default_value_;

QttpRequest::QttpRequest() :
  url_(),
  headers_(),
  body_(),
  method_(),
  timestamp_(uv_hrtime())
{
}

QttpRequest::~QttpRequest()
{
}

const QString& QttpRequest::get_header(const QString& key) const
{
  auto it = headers_.find(key);
  if(it != headers_.end()) return it->second;
  return QttpRequest::default_value_;
}

bool QttpRequest::get_header(const QString& key, QString& value) const
{
  auto it = headers_.find(key);
  if(it != headers_.end())
  {
    value = it->second;
    return true;
  }
  return false;
}

const std::map<QString, QString>& QttpRequest::get_headers() const
{
  return headers_;
}

QttpClientContext::QttpClientContext(native::net::tcp* server) :
  parser_(),
  parser_settings_(),
  was_header_value_(true),
  last_header_field_(),
  last_header_value_(),
  socket_(nullptr),
  request_(nullptr),
  response_(nullptr),
  callback_lut_(new callbacks(1))
{
  assert(server);

  // TODO: Check Error.
  //
  // TODO: Should this also toggle between SSL?

  socket_ = std::shared_ptr<native::net::tcp> (new native::net::tcp);
  server->accept(socket_.get());
}

QttpClientContext::~QttpClientContext()
{
  if(request_)
  {
    delete request_;
    request_ = nullptr;
  }

  if(response_)
  {
    delete response_;
    response_ = nullptr;
  }

  if(callback_lut_)
  {
    delete callback_lut_;
    callback_lut_ = nullptr;
  }

  if(socket_.use_count())
  {
    socket_->close([ = ](){
      PRINT_DBG("Socket closed");
    });
  }
}

bool QttpClientContext::parse(std::function<void(QttpRequest&, QttpResponse&)> callback)
{
  request_ = new QttpRequest;
  response_ = new QttpResponse(this, socket_.get());

  http_parser_init(&parser_, HTTP_REQUEST);
  parser_.data = this;

  // store callback object
  callbacks::store(callback_lut_, 0, callback);

  parser_settings_.on_url = [](http_parser* parser, const char *at, size_t len) {
                              auto client = reinterpret_cast<QttpClientContext*>(parser->data);
                              try
                              {
                                client->request_->url_.from_buf(at, len);
                              }
                              catch(const url_parse_exception& ex)
                              {
                                // from_buf() can throw an exception.
                                PRINT_STDERR(ex.message());
                              }
                              return 0;
                            };

  parser_settings_.on_header_field = [](http_parser* parser, const char* at, size_t len) {
                                       auto client = reinterpret_cast<QttpClientContext*>(parser->data);
                                       if(client->was_header_value_)
                                       {
                                         // new field started
                                         if(!client->last_header_field_.isEmpty())
                                         {
                                           // add new entry
                                           client->request_->headers_[client->last_header_field_] = std::move(client->last_header_value_);
                                         }

                                         client->last_header_field_ = QString::fromUtf8(at, len);
                                         client->was_header_value_ = false;
                                       }
                                       else
                                       {
                                         // appending
                                         client->last_header_field_ += QString::fromUtf8(at, len);
                                       }
                                       return 0;
                                     };

  parser_settings_.on_header_value = [](http_parser* parser, const char* at, size_t len) {
                                       auto client = reinterpret_cast<QttpClientContext*>(parser->data);

                                       if(!client->was_header_value_)
                                       {
                                         client->last_header_value_ = QString::fromUtf8(at, len);
                                         client->was_header_value_ = true;
                                       }
                                       else
                                       {
                                         // appending
                                         client->last_header_value_ += QString::fromUtf8(at, len);
                                       }
                                       return 0;
                                     };

  parser_settings_.on_headers_complete = [](http_parser* parser) {
                                           auto client = reinterpret_cast<QttpClientContext*>(parser->data);
                                           // add last entry if any
                                           if(!client->last_header_field_.isEmpty()) {
                                             // add new entry
                                             client->request_->headers_[client->last_header_field_] = std::move(client->last_header_value_);
                                           }
                                           return 0; // 1 to prevent reading of message body.
                                         };

  parser_settings_.on_body = [](http_parser* parser, const char* at, size_t len) {
                               PRINT_DBG("on_body: len of 'char* at' is " << len);
                               auto client = reinterpret_cast<QttpClientContext*>(parser->data);
                               client->request_->body_.append(at, len);
                               return 0;
                             };

  parser_settings_.on_message_complete = [](http_parser* parser) {
                                           PRINT_DBG("on_message_complete, so invoke the callback");
                                           auto client = reinterpret_cast<QttpClientContext*>(parser->data);
                                           client->request_->method_ = http_method_str((http_method)parser->method);
                                           // invoke stored callback object
                                           callbacks::invoke<decltype(callback)>(client->callback_lut_, 0, *client->request_, *client->response_);
                                           return 1; // 0 or 1?
                                         };

  socket_->read_start([ = ](const char* buf, int len) {
    if ((buf == nullptr) || (len < 0)) {
      response_->set_status(500);
    } else {
      http_parser_execute(&parser_, &parser_settings_, buf, len);
    }
  });

  return true;
}

Qttp::Qttp() : socket_(new native::net::tcp)
{
}

Qttp::~Qttp()
{
  if(socket_)
  {
    socket_->close([](){
      PRINT_DBG("Closing socket");
    });
  }
}

bool Qttp::listen(const std::string& ip, int port, std::function<void(QttpRequest&, QttpResponse&)> callback)
{
  if(!socket_->bind(ip, port)) {
    PRINT_STDERR("Failed to bind to ip/port " << ip << ":" << port);
    return false;
  }

  auto closed = [](){
                  PRINT_STDERR("Closing socket due to an error");
                };

  auto connected = [ = ](native::error err) {
                     if(err)
                     {
                       // TODO: handle client connection error
                       PRINT_NN_ERROR(err);
                       socket_.get()->close(closed);
                     }
                     else
                     {
                       auto client = new QttpClientContext(socket_.get());
                       client->parse(callback);
                     }
                   };

  return socket_->listen(connected);
}
