#ifndef HEART_BEAT_MANAGER_H
#define HEART_BEAT_MANAGER_H

#include <map>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include <muduo/base/Timestamp.h>
#include <muduo/base/TimerId.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Singleton.h>

typedef boost::function<void ()> ExpiredCallback;
typedef unsigned int UINT;

class HeartBeatManager : boost::noncopyable
{
public:

    //start HBWorker
    class HBWorker
    {
    public:

        HBWorker( EventLoop* loop,
                  int initial,
                  int interval,
                  int heartBeats,
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
        int initial_;
        int interval_;
        int heartBeats_;
        int T_;
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
                               string connName,
                               ExpiredCallback const& cb)
    {
        HBWorkerPtr worker( new HBWorker( loop_ , initial , interval , heartBeats,
                                          cb ) );
        auto it = workMap_.find(connName);
        if(it != workMap_.end())
            LOG_FATAL << "The connection heartbeat has been built";
        workMap_[connName] = worker;
    }

    void resetHeartBeatTask(string connName)
    {
        HBWorkerPtr worker = workerMap_[connName];
        worker->resetExpiredTime();
        worker->resetHeartBeats();
    }

    void revokeHeartBeatTask(string connName)
    {
        auto it = workerMap_.find(connName);
        if( it != workerMap_.end() )
            (it->second)->cancelTimer();
        workerMap_.erase(it);
    }

    void MessageCallBack(muduo::net::TcpConnectionPtr const& conn,
                         HeartBeatMessage const& msg,
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

#endif