#include <muduo/net/Buffer.h>
#include <muduo/base/Exception.h>

#include "HttpContext.h"

namespace muduo
{
namespace net
{

using muduo::Exception;

bool HttpContext::processRequestLine(char const* begin, char const* end,
                                     HttpContext* context)
{
    bool succeed = false;
    char const* start = begin;
    char const* space = std::find(start, end, ' ');
    HttpRequest& req = context->request();
    if (space != end && req.setMethod(string(start, space)))
    {
        start = space+1;
        space = std::find(start, end, ' ');
        if (space != end)
        {
            char const* question = std::find(start, space, '?');
            if (question != space)
            {
                req.setPath(string(start, question));
                req.setQuery(string(question, space));
            }
            else
                req.setPath(string(start, space));

            start = space+1;
            succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
            if (succeed)
            {
                if (*(end-1) == '1')
                {
                    req.setVersion(HttpRequest::Version::kHttp1dot1);
                }
                else if (*(end-1) == '0')
                {
                    req.setVersion(HttpRequest::Version::kHttp1dot0);
                }
                else
                    succeed = false;
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(Buffer* buf, HttpContext* context ,
                               Timestamp receiveTime)
{
    bool successful = true;
    bool hasMore = true;

    try
    {
        while (hasMore)
        {
            if (context->expectRequestLine())
            {
                char const* crlf = buf->findCRLF();
                if (crlf)
                {
                    successful = processRequestLine(buf->peek(), crlf, context);
                    if (successful)
                    {
                        context->request().setReceiveTime(receiveTime);
                        buf->retrieveUntil(crlf + 2);
                        context->receiveRequestLine();
                    }
                    else
                        throw Exception("processRequestLine is not successful");
                }
                else
                    throw Exception("there is not CRLF when expectRequestLine");
            }
            else if (context->expectHeaders())
            {
                char const* crlf = buf->findCRLF();
                if (crlf)
                {
                    char const* colon = std::find(buf->peek(), crlf, ':');
                    if (colon != crlf)
                    {
                        string tmp(buf->peek() , crlf);
                        context->request().setHeaders(tmp);
                    }
                    else
                    {
                        // empty line, end of header
                        context->receiveHeaders();
                        hasMore = !context->gotAll();
                    }
                    buf->retrieveUntil(crlf + 2);
                }
                else
                    throw Exception("there is not CRLF when exceptHeaders");
            }
            else if (context->expectBody())
                throw Exception("no body version right now");
        }
    }
    catch(Exception const& ex)
    {
        LOG_ERROR << "Exception is : " << ex.what()
                  << "\t"
                  << "BackTrace : " << ex.stackTrace();
        successful = false;
    }

    return successful;
}
}
}