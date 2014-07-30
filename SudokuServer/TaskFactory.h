#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <muduo/base/Singleton.h>

typedef boost::function<void (void)> TaskhandlingFunc;

class Task
{
public:

    Task(TaskhandlingFunc const& handler):handler_(handler)
    {
    }

    void operator()(void)
    {
        handler_();
    }

private:

    TaskhandlingFunc handler_;
};

typedef boost::shared_ptr<Task> TaskPtr;

class TaskFactory:boost::noncopyable
{
public:

    TaskPtr const returnATask(TaskhandlingFunc const& handler)
    {
        TaskPtr task( new Task(handler) );
        return task;
    }

private:

    TaskFactory()
    {
    }

    ~TaskFactory()
    {
    }

    friend class muduo::Singleton<TaskFactory>;
};

#endif