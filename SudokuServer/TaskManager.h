#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <muduo/base/ThreadPool.h>
#include <muduo/base/Singleton.h>

#include "TaskFactory.h"

class TaskManager:boost::noncopyable
{
public:

    void startThreadPool(int num)
    {
        threadPoll_.start(num);
    }

    void runTask(TaskPtr const& task)
    {
        threadPoll_.run(*task);
    }

private:

    muduo::ThreadPool threadPoll_;
};

#endif