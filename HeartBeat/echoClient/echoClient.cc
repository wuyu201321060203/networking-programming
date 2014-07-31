#include <muduo/net/TcpClient.h>

#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <utility>

#include <stdio.h>
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

int numThreads = 0;
class EchoClient;
boost::ptr_vector<EchoClient> clients;
int current = 0;

class EchoClient : boost::noncopyable
{
public:

    EchoClient(EventLoop* loop, InetAddress const& listenAddr, string const& id)
        : loop_(loop),
        client_(loop, listenAddr, "EchoClient"+id)
    {
        client_.setConnectionCallback(
            boost::bind(&EchoClient::onConnection, this, _1));
        client_.setMessageCallback(
            boost::bind(&EchoClient::onMessage, this, _1, _2, _3));
    }

    void connect()
    {
        client_.connect();
    }

private:

    void onConnection(TcpConnectionPtr const& conn)
    {
        LOG_TRACE << conn->localAddress().toIpPort() << " -> "
            << conn->peerAddress().toIpPort() << " is "
            << (conn->connected() ? "UP" : "DOWN");

        if (conn->connected())
        {
            ++current;
            if (implicit_cast<size_t>(current) < clients.size())
            {
                clients[current].connect();
            }
            LOG_INFO << "*** connected " << current;
        }
        conn->send("world\n");
    }

    void onMessage(TcpConnectionPtr const& conn, Buffer* buf, Timestamp time)
    {
        string msg(buf->retrieveAllAsString());
        LOG_TRACE << conn->name() << " recv " << msg.size() << " bytes at " << time.toString();
        if (msg == "quit\n")
        {
            conn->send("bye\n");
            conn->shutdown();
        }
        else if (msg == "shutdown\n")
        {
            loop_->quit();
        }
        else
        {
            conn->send(msg);
        }
    }

    EventLoop* loop_;
    TcpClient client_;
};

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    if (argc > 2)
    {
        EventLoop loop;

#pragma GCC diagnostic ignored "-Wold-style-cast"
        uint16_t port;
        port = static_cast<uint16_t>( atoi(argv[2]) );
        InetAddress serverAddr(argv[1], port);
#pragma GCC diagnostic error "-Wold-style-cast"

        int n = 1;
        if (argc > 3)
        {
            n = atoi(argv[3]);
        }

        clients.reserve(n);
        for (int i = 0; i < n; ++i)
        {
            char buf[32];
            snprintf(buf, sizeof buf, "%d", i+1);
            clients.push_back(new EchoClient(&loop, serverAddr, buf));
        }

        clients[current].connect();
        loop.loop();
    }
    else
    {
        printf("Usage: %s host_ip [current#]\n", argv[0]);
    }
}
