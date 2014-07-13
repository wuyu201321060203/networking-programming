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

    HyperProxy(muduo::net::EventLoop* loop , muduo::net::InetAddress const& proxyAddress);

    void init(std::string filePath);

    void onFrontConnection(muduo::net::TcpConnectionPtr const& conn);

    void onFrontMessage(muduo::net::TcpConnectionPtr const& conn,
                        muduo::net::Buffer* buf,
                        muduo::Timestamp recvTime);
    void start();

private:

    std::vector<TunnelPtr> tunnelVec_;
    muduo::net::TcpServer server_;
    muduo::net::EventLoop* loop_;
    ConfiguringParser parser_;

private:

    RelayMsg transformToProtoMsg(muduo::string connName , muduo::string msg);

    void initBackendFromLuaiScript(std::string filePath);
};

#endif