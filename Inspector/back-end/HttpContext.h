#ifndef HTTP_HTTPCONTEXT_H
#define HTTP_HTTPCONTEXT_H

#include <muduo/base/copyable.h>

#include "HttpRequest.h"

namespace muduo
{
namespace net
{

class HttpContext : public muduo::copyable
{
public:

    //C++11
    typedef enum class HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,

    }HttpRequestParseState;

    HttpContext()
        : state_(HttpRequestParseState::kExpectRequestLine)
    {

    }

    bool expectRequestLine() const
    {
        return state_ == HttpRequestParseState::kExpectRequestLine;
    }

    bool expectHeaders() const
    {
        return state_ == HttpRequestParseState::kExpectHeaders;
    }

    bool expectBody() const
    {
        return state_ == HttpRequestParseState::kExpectBody;
    }

    bool gotAll() const
    {
        return state_ == HttpRequestParseState::kGotAll;
    }

    void receiveRequestLine()
    {
        state_ = HttpRequestParseState::kExpectHeaders;
    }

    void receiveHeaders()
    {
        state_ = HttpRequestParseState::kGotAll;
    }

    void reset()
    {
        state_ = HttpRequestParseState::kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    HttpRequest const& request() const
    {
        return request_;
    }

    HttpRequest& request()
    {
        return request_;
    }

    bool processRequestLine(char const* begin, char const* end, HttpContext* context);
    bool parseRequest(Buffer* buf, HttpContext* context, Timestamp receiveTime);

private:
    HttpRequestParseState state_;
    HttpRequest request_;
};

}
}

#endif