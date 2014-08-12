#include <boost/bind.hpp>

#include <muduo/base/Logging.h>

#include "echo.h"

EchoServer::EchoServer(muduo::net::EventLoop* loop,
                       muduo::net::InetAddress const& listenAddr)
                       : loop_(loop),
                         server_(loop, listenAddr, "EchoServer"),
                         dispatcher_(boost::bind(&EchoServer::onUnknownMessage,
                                                this , _1 , _2 , _3)),
                         codec_(boost::bind(&ProtobufDispatcher::onProtobufMessage,
                                            &dispatcher_, _1 , _2 , _3))
{
    dispatcher_.registerCallback<EchoMessage>(
            boost::bind(&EchoServer::onEchoMessage , this , _1 , _2 , _3));

    dispatcher_.registerCallback<HeartBeatMessage>(
            boost::bind(&HeartBeatManager::onMessageCallback,
            &manager_ , _1 , _2 , _3));

    server_.setConnectionCallback(
        boost::bind(&EchoServer::onConnection, this, _1));

    server_.setMessageCallback(
        boost::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));

    manager_.setEventLoop(loop_);
}

void EchoServer::start()
{
    server_.start();
}

void EchoServer::onConnection(muduo::net::TcpConnectionPtr const& conn)
{
    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

    if(conn->connected())
    {
        manager_.delegateTimerTask(60 , 10 , 3,
            boost::bind(&EchoServer::onHeartBeatMessage , this),
            conn);
    }
}

void EchoServer::onEchoMessage(muduo::net::TcpConnectionPtr const& conn,
                               EchoMsgPtr const& msg,
                               muduo::Timestamp time)
{
    LOG_INFO << conn->name() << " echo " << msg->ByteSize() << " bytes, "
        << "data received at " << time.toString();
    manager_.resetTimerTask(conn);
    EchoMessage message;
    message.set_msg(msg->msg());
    codec_.send(conn , message);
}

void EchoServer::onHeartBeatMessage()
{
    LOG_INFO << "peer is down down down :-)";
    return;
}

void EchoServer::onUnknownMessage(muduo::net::TcpConnectionPtr const& conn,
                                  MessagePtr const& msg,
                                  muduo::Timestamp receiveTime)
{
    LOG_INFO << "onUnknownMessage:" << msg->GetTypeName();
}
