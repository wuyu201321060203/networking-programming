#ifndef HEART_BEAT_MANAGER_H
#define HEART_BEAT_MANAGER_H

#include <map>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <muduo/base/Timestamp.h>
#include <muduo/net/TimerId.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

#include "Config.h"
#include "util.h"
#include "EchoMessage.pb.h"

typedef boost::function<void ()> TimeoutCallback;
typedef unsigned int uint;
typedef boost::shared_ptr<HeartBeatMessage> HBMsgPtr;

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

        TcpConnectionWeakPtr getConnWeakPtr()
        {
            return _conn;
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
                           HBMsgPtr const& msg,
                           muduo::Timestamp timestamp)
    {
        resetTimerTask(conn);
        muduo::net::Buffer buf;
        EchoMessage message;
        message.set_msg(msg->msg());
        ProtobufCodec::fillEmptyBuffer(&buf , message);
        conn->send(&buf);
    }

    void getDCList(TcpConnectionWeakPtrVec& dcList)
    {
        for(auto it = _beaterMap.begin() ; it != _beaterMap.end() ; ++it)
        {
            dcList.push_back( (it->second)->getConnWeakPtr() );
        }
    }

private:

    muduo::net::EventLoop* _loop;
    Name2BeaterMap _beaterMap;
};

#endif