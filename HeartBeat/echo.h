#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <boost/shared_ptr.hpp>

#include <muduo/net/TcpServer.h>

#include "codec.h"
#include "ProtobufDispatcher.h"
#include "EchoMessage.pb.h"

typedef boost::shared_ptr<EchoMessage> EchoMsgPtr;
typedef boost::shared_ptr<HeartBeatMessage> HBMsgPtr;

// RFC 862
class EchoServer
{
public:

    EchoServer(muduo::net::EventLoop* loop,
               muduo::net::InetAddress const& listenAddr);

    void start();

private:

    void onConnection(muduo::net::TcpConnectionPtr const& conn);

    void onEchoMessage(muduo::net::TcpConnectionPtr const& conn,
                   EchoMsgPtr const& msg,
                   muduo::Timestamp time);

    void onHeartBeatMessage();

    void onUnknownMessage(muduo::net::TcpConnectionPtr const& conn,
                          MessagePtr const& meg,
                          muduo::Timestamp receiveTime);

    muduo::net::EventLoop* loop_;
    muduo::net::TcpServer server_;
    ProtobufDispatcher dispatcher_;
    ProtobufCodec codec_;
};

#endif
