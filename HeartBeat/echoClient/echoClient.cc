#include <utility>
#include <stdio.h>
#include <unistd.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/program_options.hpp>

#include <muduo/net/TcpClient.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include "codec.h"
#include "ProtobufDispatcher.h"
#include "EchoMessage.pb.h"
#include "HeartBeatManager.h"

using namespace muduo;
using namespace muduo::net;

namespace po = boost::program_options;

typedef boost::shared_ptr<EchoMessage> EchoMsgPtr;

int numThreads = 0;
class EchoClient;
boost::ptr_vector<EchoClient> clients;
int current = 0;

class EchoClient : boost::noncopyable
{
public:

    EchoClient(EventLoop* loop, InetAddress const& listenAddr, string const& id)
        : loop_(loop),
          client_(loop, listenAddr, "EchoClient"+id),
          dispatcher_(boost::bind(&EchoClient::onUnknownMessage,
                                  this , _1 , _2 , _3)),
          codec_(boost::bind(&ProtobufDispatcher::onProtobufMessage,
                             &dispatcher_, _1 , _2 , _3))

    {
        dispatcher_.registerCallback<EchoMessage>(
            boost::bind(&EchoClient::onMessage , this , _1 , _2 , _3));
        dispatcher_.registerCallback<HeartBeatMessage>(
            boost::bind(&HeartBeatManager::onMessageCallback,
            &manager_ , _1 , _2 , _3));

        client_.setConnectionCallback(
            boost::bind(&EchoClient::onConnection, this, _1));

        client_.setMessageCallback(
            boost::bind(&ProtobufCodec::onMessage, &codec_, _1, _2, _3));

        manager_.setEventLoop(loop_);
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
            manager_.delegateTimerTask(50 , 10 , 3,
                boost::bind(&EchoClient::onHeartBeatMessage , this , conn),
                conn);

            ++current;
            if (implicit_cast<size_t>(current) < clients.size())
            {
                clients[current].connect();
            }
            LOG_INFO << "*** connected " << current;
        }
        EchoMessage message;
        message.set_msg("helloworld");
        codec_.send(conn , message);
    }

    void onMessage(TcpConnectionPtr const& conn, EchoMsgPtr const& msg, Timestamp time)
    {
        manager_.resetTimerTask(conn);
        LOG_TRACE << conn->name() << " recv " << msg->ByteSize() << " bytes at " << time.toString();
        EchoMessage message;
        message.set_msg(msg->msg());
        codec_.send(conn , message);
    }

    void onHeartBeatMessage(TcpConnectionPtr const& conn)
    {
        HeartBeatMessage message;
        message.set_msg("bingo");
        codec_.send(conn , message);
    }

    void onUnknownMessage(TcpConnectionPtr const& conn,
                          MessagePtr const& msg,
                          Timestamp receiveTime)
    {
         LOG_INFO << "onUnknownMessage:" << msg->GetTypeName();
    }

    EventLoop* loop_;
    TcpClient client_;
    ProtobufDispatcher dispatcher_;
    ProtobufCodec codec_;
    HeartBeatManager manager_;
};


bool parseCommandLine(int argc , char* argv[] , int* clientNum , string* ip,
                      uint16_t* port)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help")
        ("port,p", po::value<uint16_t>(port), "TCP port")
        ("ip,i" , po::value<string>(ip), "ip of server")
        ("clients,n" , po::value<int>(clientNum), "num of clients");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if(vm.count("help"))
    {
        std::cout << "\n";
        std::cout << desc << "\n";
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    int n = 1;
    string ip = "127.0.0.1";
    uint16_t port = 9877;
    if(parseCommandLine(argc , argv , &n , &ip , &port))
    {
        EventLoop loop;
        InetAddress serverAddr(ip , port);
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
    return 0;
}
