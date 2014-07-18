#include <boost/bind.hpp>

#include <muduo/base/Logging.h>
#include <muduo/base/Exception.h>

#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

using namespace muduo;
using namespace muduo::net;
using muduo::Exception;

namespace muduo
{
namespace net
{
namespace detail
{

void defaultHttpCallback(HttpRequest const&, HttpResponse& resp)
{
    resp.setStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
    resp.setStatusMessage("Not Found");
    resp.setCloseConnection(true);
}

}
}
}

HttpServer::HttpServer(EventLoop* loop,
    InetAddress const& listenAddr,
    string const& name,
    TcpServer::Option option)
: server_(loop, listenAddr, name, option),
    httpCallback_(detail::defaultHttpCallback)
{
    server_.setConnectionCallback(
        boost::bind(&HttpServer::onConnection, this, _1));
    server_.setMessageCallback(
        boost::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << server_.name()
        << "] starts listenning on " << server_.hostport();
    server_.start();
}

void HttpServer::onConnection(TcpConnectionPtr const& conn)
{
    if (conn->connected())
    {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(TcpConnectionPtr const& conn,
    Buffer* buf,
    Timestamp receiveTime)
{
    HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());

    if (!context->parseRequest(buf, context, receiveTime))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll())
    {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(TcpConnectionPtr const& conn, HttpRequest const& req)
{
    string const& connection = req.getContentByHeader("Connection");
    bool close = (connection == "close");
    HttpResponse response(close);
    httpCallback_(req, response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection())
    {
        conn->shutdown();
    }
}