#include "echo.h"

#include <boost/program_options.hpp>

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>

// using namespace muduo;
// using namespace muduo::net;
namespace po = boost::program_options;

bool parseCommandLine(int argc , char* argv[] , uint16_t* port)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help")
        ("port,p", po::value<uint16_t>(port), "TCP port");
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

int main(int argc , char* argv[])
{
    LOG_INFO << "pid = " << getpid();
    muduo::net::EventLoop loop;
    uint16_t port = 9877;
    if(parseCommandLine(argc , argv , &port))
    {
        muduo::net::InetAddress listenAddr(port);
        EchoServer server(&loop, listenAddr);
        server.start();
        loop.loop();
    }
}
