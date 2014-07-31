#include <muduo/base/Logging.h>

#include <boost/bind.hpp>

#include "echo.h"
#include "HeartBeatManager.h"

EchoServer::EchoServer(muduo::net::EventLoop* loop,
                       muduo::net::InetAddress const& listenAddr)
                      : loop_(loop),
                        server_(loop, listenAddr, "EchoServer"),
                        dispatcher_(boost::bind(&EchoServer::onUnknownMessage,
                                                this , _1 , _2 , _3)),
                        codec_(boost::bind(&ProtobufDispatcher::onProtobufMessage,
                                            &dispatcher, _1 , _2 , _3))
{
    dispatcher_.registerCallback<EchoServer::Echo>(boost::bind(&EchoServer::onMessage,
        this , _1 , _2 , _3));

    dispatcher_.registerCallback<EchoServer::HeartBeat>(
            &HeartBeatManager::MessageCallback,
            &SingleHB::instance(),
            _1 , _2 , _3);

    server_.setConnectionCallback(
        boost::bind(&EchoServer::onConnection, this, _1));

    server_.setMessageCallback(
        boost::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));
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
    SingleHB::instance().setEventLoop(loop_);
    SingleHB::instance().delegateHeartBeatTask(60 , 10 , 3 , conn->name(),
        boost::bind(&EchoServer::onHeartBeatMessage , this),
        conn);
}

void EchoServer::onMessage(muduo::net::TcpConnectionPtr const& conn,
                           EchoPtr const& msg,
                           muduo::Timestamp time)
{
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
        << "data received at " << time.toString();
    codec_.send(conn , msg);
}

void EchoServer::onUnknownMessage(TcpConnectionPtr const& conn,
                                  MessagePtr const& msg,
                                  Timestamp receiveTime)
{
    LOG_INFO << "onUnknownMessage:" << msg->GetTypeName();
}

