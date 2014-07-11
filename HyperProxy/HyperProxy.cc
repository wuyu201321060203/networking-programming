#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

#include "Tunnel.h"
#include "codec.h"

#include "RelayMsg.pb.h"

#include "HyperProxy.h"

using namespace muduo;
using namespace muduo::net;

std::map<string , TcpConnectionPtr> g_nameService;

HyperProxy::HyperProxy(EventLoop* loop , InetAddress const& proxyAddress):
                       server_(loop , proxyAddress , "HyperProxy"),
                       loop_(loop)
{
    server_.setConnectionCallback( boost::bind(&HyperProxy::onFrontConnection,
        this , _1) );
    server_.setMessageCallback( boost::bind(&HyperProxy::onFrontMessage,
        this , _1 , _2 , _3) );

}

void HyperProxy::initBackendFromLuaiScript()
{
    char const* ip = "127.0.0.1";
    TunnelPtr aTunnel(new Tunnel(loop_ , InetAddress(ip , 12345)));
    TunnelPtr bTunnel(new Tunnel(loop_ , InetAddress(ip , 9877)));
    tunnelVec_.push_back(aTunnel);
    tunnelVec_.push_back(bTunnel);
}

void HyperProxy::init()
{
    initBackendFromLuaiScript();
    for(auto it = tunnelVec_.begin() ; it != tunnelVec_.end() ; ++it)
    {
        (*it)->connect();
    }
}

void HyperProxy::start()
{
    server_.start();
}

void HyperProxy::onFrontConnection(TcpConnectionPtr const& conn)
{
    static int roundRobinPick = 0;
    LOG_DEBUG << (conn->connected() ? "Up" : "Down");
    if(conn->connected())
    {
        conn->setContext( ( tunnelVec_[ (roundRobinPick++) % tunnelVec_.size() ] )
            ->getBackendConn() );

        auto it = g_nameService.find(conn->name());
        if( it != g_nameService.end() )
            LOG_FATAL << "there are identical names for different Tcpconnection\n";
        g_nameService[conn->name()] = conn;
    }
    else
    {
        g_nameService.erase(conn->name());
    }
}

void HyperProxy::onFrontMessage(TcpConnectionPtr const& conn,
                                Buffer* buf,
                                Timestamp recvTime)
{
    string msg = buf->retrieveAllAsString();
    string connName = conn->name();
    RelayMsg const protoMsg = transformToProtoMsg(connName , msg);
    TcpConnectionPtr const& clientConn =
        boost::any_cast<TcpConnectionPtr const&>(conn->getContext());
    Buffer buffer;
    ProtobufCodec::fillEmptyBuffer(&buffer , protoMsg);
    clientConn->send(&buffer);
}

RelayMsg HyperProxy::transformToProtoMsg(string connName , string msg)
{
    RelayMsg relayMsg;
    relayMsg.set_connname(connName.c_str());
    relayMsg.set_msg(msg.c_str());
    return relayMsg;
}