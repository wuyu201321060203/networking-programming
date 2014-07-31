#ifndef HEART_BEAT_MANAGER_H
#define HEART_BEAT_MANAGER_H

#include <map>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <muduo/base/Timestamp.h>
#include <muduo/net/TimerId.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Singleton.h>

#include "EchoMessage.pb.h"

typedef boost::function<void ()> ExpiredCallback;
typedef unsigned int UINT;

using muduo;
using muduo::net;

class HeartBeatManager : boost::noncopyable
{
public:

    //start HBWorker
    class HBWorker
    {
    public:

        HBWorker( EventLoop* loop,
                  UINT initial,
                  UINT interval,
                  UINT heartBeats,
                  ExpiredCallback cb,
                  TcpConnectionPtr const& conn
                ):loop_(loop),
                  initial_(initial),
                  interval_(interval),
                  heartBeats_(heartBeats),
                  T_(0),
                  callback_(cb),
                  cancelled_(false),
                  conn_(conn)
        {
            timerId_ = loop->runAfter( initial_ , boost::bind(&ExpiredCallback,
                                       this) );
        }

        ~HBWorker()//取消定时器
        {
            if(!cancelled_)
                cancelTimer();
        }

        void ExpiredCallback()
        {
            if(T_++ > heartBeats)
                LOG_INFO << conn->name() << "may be down";
            callback_();
            timerId_ = loop->runAfter( interval , boost::bind(&ExpiredCallback,
                                       this) );
        }

        void resetExpiredTime()
        {
            cancelTimer();
            timerId_ = loop->runAfter( initial_ , boost::bind(&ExpiredCallback,
                                       this ) );
        }

        void resetHeartBeats()
        {
            T_ = 0;
        }

        void cancelTimer()
        {
            loop->cancel(timerId_);
        }

    private:

        EventLoop* loop_;
        UINT initial_;
        UINT interval_;
        UINT heartBeats_;
        UINT T_;
        ExpiredCallback callback_;
        bool cancelled_;
        TcpConnectionPtr conn_;
        TimerId timerId_;
    };

    typedef boost::shared_ptr<HBWorker> HBWorkerPtr;
    //end HBWorker

    void setEventLoop(EventLoop loop*)
    {
        loop_ = loop;
    }

    void delegateHeartBeatTask(UINT initial,  //seconds
                               UINT interval, //seconds
                               UINT heartBeats,
                               ExpiredCallback const& cb,
                               TcpConnectionPtr const& conn)
    {
        HBWorkerPtr worker( new HBWorker( loop_ , initial , interval , heartBeats,
                                          cb , conn) );
        string connName = conn->name();
        auto it = workMap_.find(connName);
        if(it != workMap_.end())
            LOG_FATAL << "The connection heartbeat has been built";
        workMap_[connName] = worker;
    }

    void resetHeartBeatTask(TcpConnectionPtr const& conn)
    {
        string connName = conn->name();
        HBWorkerPtr worker = workerMap_[connName];
        worker->resetExpiredTime();
        worker->resetHeartBeats();
    }

    void revokeHeartBeatTask(TcpConnectionPtr const& conn)
    {
        string connName = conn->name();
        auto it = workerMap_.find(connName);
        if( it != workerMap_.end() )
            (it->second)->cancelTimer();
        workerMap_.erase(it);
    }

    void MessageCallBack(muduo::net::TcpConnectionPtr const& conn,
                         EchoServer::HeartBeatMessage const& msg,
                         muduo::Timestamp timestamp)
    {
        resetHeartBeatTask( conn->name() );
        muduo::net::Buffer buf;
        Protobuf::fileEmptyBuf(&buf , msg);
        conn->send(buf);
    }

private:

    HeartBeatManager()
    {
    }

    ~HeartBeatManager()
    {
    }

    friend class muduo::Singleton<HeartBeatManager>;

private:

    EventLoop* loop_;
    std::map<string , HBWorkerPtr> workerMap_;
};

typedef muduo::Singleton<HeartManager> SingleHB;

#endif