#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <muduo/base/ThreadPool.h>

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

    TaskManager()
    {
    }

    ~TaskManager()
    {
    }

private:

    friend class muduo::Singleton<TaskManager>;
    muduo::ThreadPool threadPoll_;
};

#endif