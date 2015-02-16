#ifndef HEART_BEAT_MANAGER_H
#define HEART_BEAT_MANAGER_H

#include <map>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <muduo/base/Timestamp.h>
#include <muduo/net/TimerId.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

#include "util.h"

typedef boost::function<void ()> TimeoutCallback;
typedef boost::function<void (muduo::net::TcpConnectionPtr const&,
                              MessagePtr const&,
                              muduo::Timestamp)> onTimeCallback;
typedef unsigned int uint;
typedef std::string STDSTR;

class HeartBeatManager : boost::noncopyable
{
public:

    class HeartBeater
    {
    public:

        HeartBeater(muduo::net::EventLoop* loop,
                    uint startTime,
                    uint timeInterval,
                    uint ttl,
                    TimeoutCallback const& cb,
                    muduo::net::TcpConnectionPtr const& conn,
                    HeartBeatManager* manager
                    ):
                        _loop(loop),
                        _startTime(startTime),
                        _timeInterval(timeInterval),
                        _ttl(ttl),
                        _counter(0),
                        _callback(cb),
                        _conn(conn),
                        _manager(manager)
        {
            _taskID = getTimerTask(_startTime);
        }

        ~HeartBeater()
        {
            cancelTimerTask();
        }

        void onTimeout()
        {
            if(_counter++ > _ttl)
            {
                LOG_INFO << _conn->peerAddress().toIpPort() << " may be down";//TODO
                _callback();
                _manager->deleteTimerTask(_conn);
            }
            else
                _taskID = getTimerTask(_timeInterval);
        }

        void resetTimeout()
        {
            _counter = 0;
            cancelTimerTask();
            _taskID = getTimerTask(_startTime);
        }

        void cancelTimerTask()
        {
            _loop->cancel(_taskID);
        }

    private:

        muduo::net::EventLoop* _loop;
        uint _startTime;
        uint _timeInterval;
        uint _ttl;
        uint _counter;
        TimeoutCallback _callback;
        muduo::net::TcpConnectionPtr _conn;
        muduo::net::TimerId _taskID;
        HeartBeatManager* _manager;

    private:

        muduo::net::TimerId getTimerTask(uint when)
        {
            return ( _loop->runAfter(when,
                boost::bind(&HeartBeatManager::HeartBeater::onTimeout,
                this) )
                );
        }
    };

    typedef boost::shared_ptr<HeartBeater> HeartBeaterPtr;
    typedef std::map<STDSTR , HeartBeaterPtr> Name2BeaterMap;

    HeartBeatManager(muduo::net::EventLoop* loop,
                     onTimeCallback const& cb):
                     _loop(loop),
                     _onTimeCallback(cb)
    {
    }

    void setEventLoop(muduo::net::EventLoop* loop)
    {
        _loop = loop;
    }

    void delegateTimerTask(uint startTime,  //seconds
                           uint timeInterval, //seconds
                           uint ttl,
                           TimeoutCallback const& cb,
                           muduo::net::TcpConnectionPtr const& conn)
    {
        HeartBeaterPtr beater( new HeartBeater( _loop , startTime , timeInterval,
            ttl , cb , conn , this) );
        STDSTR const connName = MuduoStr2StdStr(conn->name());
        _beaterMap.insert(Name2BeaterMap::value_type(connName , beater));
    }

    void resetTimerTask(muduo::net::TcpConnectionPtr const& conn)
    {
        STDSTR connName = MuduoStr2StdStr(conn->name());
        auto it = _beaterMap.find(connName);
        if(it != _beaterMap.end())
            ( it->second )->resetTimeout();
    }

    void deleteTimerTask(muduo::net::TcpConnectionPtr const& conn)
    {
        for(auto it = _beaterMap.begin() ; it != _beaterMap.end();)
        {
            if(it->first == MuduoStr2StdStr(conn->name()))
            {
                _beaterMap.erase(it++);
            }
            else
                ++it;
        }
    }

    void onMessageCallback(muduo::net::TcpConnectionPtr const& conn,
                           MessagePtr const& msg,
                           muduo::Timestamp time)
    {
        resetTimerTask(conn);
        _onTimeCallback(conn , msg , time);
    }

private:

    muduo::net::EventLoop* _loop;
    Name2BeaterMap _beaterMap;
    onTimeCallback _onTimeCallback;
};

#endif