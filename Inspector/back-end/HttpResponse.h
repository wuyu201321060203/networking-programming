#ifndef HTTP_HTTPRESPONSE_H
#define HTTP_HTTPRESPONSE_H

#include <map>

#include <muduo/base/copyable.h>
#include <muduo/base/Types.h>

namespace muduo
{
namespace net
{

class Buffer;

class HttpResponse : public muduo::copyable
{

public:

    typedef enum class HttpStatusCode
    {
        Unknown,
        k200Ok = 200,
        k301MovedPermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,

    }HttpStatusCode;

    explicit HttpResponse(bool close)
        : statusCode_(HttpStatusCode::Unknown),
        closeConnection_(close)
    {
    }

    void setStatusCode(HttpStatusCode const& code)
    { statusCode_ = code; }

    void setStatusMessage(string const& message)
    { statusMessage_ = message; }

    void setCloseConnection(bool on)
    { closeConnection_ = on; }

    bool closeConnection() const
    { return closeConnection_; }

    void setContentType(string const& contentType)
    { setHeader("Content-Type", contentType); }

    void setHeader(const string& key, string const& value)
    { headers_[key] = value; }

    void setBody(const string& body)
    { body_ = body; }

    void appendToBuffer(Buffer* output) const;

private:
    std::map<string, string> headers_;
    HttpStatusCode statusCode_;
    // FIXME: add http version
    string statusMessage_;
    bool closeConnection_;
    string body_;
};
}
}

#endif