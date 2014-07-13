#include <malloc.h>
#include <stdio.h>
#include <sys/resource.h>

#include "Tunnel.h"
#include "HyperProxy.h"

#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

void memstat()
{
  malloc_stats();
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
    uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
    InetAddress listenAddr(port);
    EventLoop loop;
    HyperProxy proxy(&loop , listenAddr);
    proxy.init("config.lua");
    proxy.start();
    loop.runEvery(3.0 , &memstat);
    loop.loop();
}