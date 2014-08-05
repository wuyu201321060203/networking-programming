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

typedef boost::function<void ()> TimeoutCallback;
typedef unsigned int UINT;

using namespace muduo;
using namespace muduo::net;

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
                  TimeoutCallback const& cb,
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
            timerId_ = getCronJob(initial_);
        }

        ~HBWorker()//取消定时器
        {
            if(!cancelled_)
                cancelTimer();
        }

        void onExpiredCallback()
        {
            if(T_++ > heartBeats_)
                LOG_INFO << conn_->peerAddress().toIpPort() << " may be down";
            callback_();
            timerId_ = getCronJob(interval_);
        }

        void resetExpiredTime()
        {
            cancelTimer();
            timerId_ = getCronJob(initial_);
        }

        void resetHeartBeats()
        {
            T_ = 0;
        }

        void cancelTimer()
        {
            loop_->cancel(timerId_);
        }

    private:

        EventLoop* loop_;
        UINT initial_;
        UINT interval_;
        UINT heartBeats_;
        UINT T_;
        TimeoutCallback callback_;
        bool cancelled_;
        TcpConnectionPtr conn_;
        TimerId timerId_;

    private:

        TimerId getCronJob(UINT when)
        {
            return ( loop_->runAfter(when ,
                boost::bind(&HeartBeatManager::HBWorker::onExpiredCallback , this)
                                    ) );
        }
    };

    typedef boost::shared_ptr<HBWorker> HBWorkerPtr;
    //end HBWorker

    void setEventLoop(EventLoop* loop)
    {
        loop_ = loop;
    }

    void delegateHeartBeatTask(UINT initial,  //seconds
                               UINT interval, //seconds
                               UINT heartBeats,
                               TimeoutCallback const& cb,
                               TcpConnectionPtr const& conn)
    {
        HBWorkerPtr worker( new HBWorker( loop_ , initial , interval , heartBeats,
                                          cb , conn) );
        string connName = conn->name();
        auto it = workerMap_.find(connName);
        if(it != workerMap_.end())
            LOG_FATAL << "The connection heartbeat has been built";
        workerMap_[connName] = worker;
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

    void onMessageCallback(muduo::net::TcpConnectionPtr const& conn,
                           HBMsgPtr const& msg,
                           muduo::Timestamp timestamp)
    {
        resetHeartBeatTask(conn);
        muduo::net::Buffer buf;
        EchoMessage message;
        message.set_msg(msg->msg());
        ProtobufCodec::fillEmptyBuffer(&buf , message);
        conn->send(&buf);
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

typedef muduo::Singleton<HeartBeatManager> SingleHB;

#endif