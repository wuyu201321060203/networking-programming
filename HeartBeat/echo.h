#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <muduo/net/TcpServer.h>

#include "codec.h"
#include "ProtobufDispatcher.h"

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
                 muduo::net::Buffer* buf,
                 muduo::Timestamp time);

  void onUnknwonMessage(TcpConnectionPtr const& conn , MessagePtr const& meg,
                        Timestamp receiveTime);

  muduo::net::TcpServer server_;
  ProtobufCodec codec_;
  ProtobufDispatcher dispatcher_;
};

#endif
