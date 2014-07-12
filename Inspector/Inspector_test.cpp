#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/base/CurrentThread.h>

#include "Inspector.h"

using namespace muduo;
using namespace muduo::net;

int g_InspectorPid = -1;

void setInspectorPid(EventLoop* rhs)
{
    g_InspectorPid = muduo::CurrentThread::tid();
}

int main(void)
{
    EventLoopThread::ThreadInitCallback func(&setInspectorPid);
    EventLoopThread t(func);
    Inspector ins(t.startLoop(), InetAddress(12345), "test");

    for(;;)
    {}
    return 0;
}
