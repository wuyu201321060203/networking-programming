#ifndef TUNNEL_H
#define TUNNEL_H

#include <map>
#include <vector>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/Callbacks.h>

#include "codec.h"
#include "RelayMsg.pb.h"

extern std::map<muduo::string , muduo::net::TcpConnectionPtr> g_nameService;

typedef boost::shared_ptr<RelayMsg> RelayMsgPtr;
typedef boost::weak_ptr<muduo::net::TcpConnection> TcpConnectionWeakPtr;

class Tunnel:public  boost::enable_shared_from_this<Tunnel>,
             private boost::noncopyable
{
public:

    Tunnel(muduo::net::EventLoop* loop , muduo::net::InetAddress backendAddr):
        client_(loop , backendAddr , "TunnelClient"),
        codec_( boost::bind(&Tunnel::onBackendMessage , this , _1 , _2 , _3) )
    {
        LOG_INFO << "Tunnel";
        client_.setConnectionCallback(boost::bind(&Tunnel::onBackendConnection ,
            this , _1));
        client_.setMessageCallback(boost::bind(&ProtobufCodec::onMessage,
            &codec_ , _1 , _2 , _3));

    }

private:

    void onBackendConnection(muduo::net::TcpConnectionPtr const& conn)
    {
        LOG_DEBUG << (conn->connected() ? "Up" : "Down");
        if(conn->connected())
        {
            backendConn_ = conn;
        }
        else
        {
            client_.setConnectionCallback(muduo::net::defaultConnectionCallback);
            client_.setMessageCallback(muduo::net::defaultMessageCallback);
            for(auto item : frontConnVec_)
            {
                muduo::net::TcpConnectionPtr frontConn = item.lock();
                if(frontConn)
                {
                    frontConn->setContext(boost::any());
                    frontConn->shutdown();
                }
            }
            LOG_ERROR << "long-time connection is breakdown";
        }
    }

    void onBackendMessage(muduo::net::TcpConnectionPtr const& conn,
        MessagePtr const& msg,
        muduo::Timestamp recvTime)
    {
        RelayMsgPtr relayMsg = muduo::down_pointer_cast<RelayMsg>(msg);
        std::string connName = relayMsg->connname();
        auto it = g_nameService.find(muduo::string(connName.c_str()));
        if(it != g_nameService.end())
        {
            std::string backendMsg = relayMsg->msg();
            g_nameService[muduo::string(connName.c_str())]->send(backendMsg);
        }
    }

public:

    void connect()
    {
        client_.connect();
    }

    muduo::net::TcpConnectionPtr const& getBackendConn()
    {
        return backendConn_;
    }

    void registerFrontConnection(muduo::net::TcpConnectionPtr const& conn)
    {
        TcpConnectionWeakPtr temp(conn);
        frontConnVec_.push_back(temp);
    }

private:

    muduo::net::TcpClient client_;
    ProtobufCodec codec_;
    muduo::net::TcpConnectionPtr backendConn_;
    std::vector<TcpConnectionWeakPtr> frontConnVec_;
};

typedef boost::shared_ptr<Tunnel> TunnelPtr;

#endif