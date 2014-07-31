#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <muduo/net/TcpServer.h>

#include <boost/shared_ptr.hpp>

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

    void onMessage(muduo::net::TcpConnectionPtr const& conn,
                   EchoMsgPtr const& msg,
                   muduo::Timestamp time);

    void onHeartBeatMessage();

    void onUnknwonMessage(TcpConnectionPtr const& conn , MessagePtr const& meg,
                          Timestamp receiveTime);

    EventLoop* loop_;
    muduo::net::TcpServer server_;
    ProtobufCodec codec_;
    ProtobufDispatcher dispatcher_;
};

#endif
