#ifndef HTTP_HTTPSERVER_H
#define HTTP_HTTPSERVER_H

#include <muduo/net/TcpServer.h>
#include <boost/noncopyable.hpp>

namespace muduo
{
namespace net
{

class HttpRequest;
class HttpResponse;

class HttpServer : boost::noncopyable
{
public:

    typedef boost::function<void (HttpRequest const&,
        HttpResponse&)> HttpCallback;

    HttpServer(EventLoop* loop,
        InetAddress const& listenAddr,
        string const&  name,
        TcpServer::Option option = TcpServer::kNoReusePort);

    ~HttpServer();

    EventLoop* getLoop() const { return server_.getLoop(); }

    /// Not thread safe, callback be registered before calling start().
    void setHttpCallback(HttpCallback const& cb)
    {
        //mutexLock is unnecessary
        httpCallback_ = cb;
    }

    void setThreadNum(int numThreads)
    {
        server_.setThreadNum(numThreads);
    }

    void start();

private:
    void onConnection(TcpConnectionPtr const& conn);
    void onMessage(TcpConnectionPtr const& conn,
        Buffer* buf,
        Timestamp receiveTime);
    void onRequest(TcpConnectionPtr const&, HttpRequest const&);

    TcpServer server_;
    HttpCallback httpCallback_;
};
}
}

#endif