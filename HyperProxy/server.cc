#include <malloc.h>
#include <stdio.h>
#include <sys/resource.h>

#include "Tunnel.h"
#include "HyperProxy.h"

#include <boost/program_options.hpp>

#include <muduo/net/EventLoop.h>

#ifndef NO_DEBUG
#include <iostream>
#endif

using namespace muduo;
using namespace muduo::net;

namespace po = boost::program_options;

void memstat()
{
  malloc_stats();
}

bool parseCommandLine(int argc , char* argv[] , HyperProxy::Options& options)
{
    options.port = 9999;
    options.configfilePath = "./config.lua";
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Help")
        ("port,p", po::value<uint16_t>(&options.port), "TCP port")
        ("configfilePath,c" , po::value<string>(&options.configfilePath),
         "Configure File Path");
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
    {
        // set max virtual memory to 256MB.
        size_t kOneMB = 1024*1024;
        rlimit rl = { 256*kOneMB, 256*kOneMB };
        setrlimit(RLIMIT_AS, &rl);
    }

    HyperProxy::Options options;
    if(parseCommandLine(argc , argv , options))
    {
        EventLoop loop;
        HyperProxy proxy(&loop , options);
        proxy.init();
        proxy.start();
        loop.runEvery(3.0 , &memstat);
        loop.loop();
    }
    return 0;//normally never return
}