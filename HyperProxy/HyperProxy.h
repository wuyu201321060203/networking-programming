#ifndef HYPERPROXY_H
#define HYPERPROXY_H

#include <vector>
#include <map>
#include <string>

#include <boost/noncopyable.hpp>

#include <muduo/base/Timestamp.h>

#include "Tunnel.h"
#include "codec.h"
#include "ConfiguringParser.h"

class HyperProxy:boost::noncopyable
{
public:

    struct Options
    {
        Options();
        uint16_t port;
        muduo::string configfilePath;
    };

    HyperProxy(muduo::net::EventLoop* loop , Options const& options);

    void init();
    void start();

private:

    std::vector<TunnelPtr> tunnelVec_;
    muduo::net::EventLoop* loop_;
    Options options_;
    muduo::net::TcpServer server_;
    ConfiguringParser parser_;

private:

    void onFrontConnection(muduo::net::TcpConnectionPtr const& conn);
    void onFrontMessage(muduo::net::TcpConnectionPtr const& conn,
                        muduo::net::Buffer* buf,
                        muduo::Timestamp recvTime);

    RelayMsg transformToProtoMsg(muduo::string connName , muduo::string msg);

    void initBackendFromLuaiScript();
};

#endif