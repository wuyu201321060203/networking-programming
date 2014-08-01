#ifndef PROTOBUF_DISPATCHER_H
#define PRoTOBUF_DISPATCHER_H

#include <map>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <google/protobuf/message.h>

#include <muduo/net/Callbacks.h>

typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

struct Callback:boost::noncopyable
{
public:

    virtual ~Callback() {}
    virtual void onMessage(muduo::net::TcpConnectionPtr const& conn,
                           MessagePtr const& message,
                           muduo::Timestamp timeStamp) const = 0;
};

template<typename T>
struct CallbackT:public Callback
{
public:

    BOOST_STATIC_ASSERT(boost::is_base_of<google::protobuf::Message , T>::value);

    typedef boost::function<void (muduo::net::TcpConnectionPtr const& ,
                            boost::shared_ptr<T> const& ,
                            muduo::Timestamp)> ConcreteCallback;

    CallbackT(ConcreteCallback cb):protobufCallback_(cb)
    {}

    virtual void onMessage(muduo::net::TcpConnectionPtr const& conn,
                      MessagePtr const& message,
                      muduo::Timestamp timeStamp) const
    {
        boost::shared_ptr<T> const& concreteMessagePtr =
                            muduo::down_pointer_cast<T>(message);

        assert(concreteMessagePtr != NULL);
        protobufCallback_(conn , concreteMessagePtr , timeStamp);
    }

private:
        ConcreteCallback protobufCallback_;
};

struct ProtobufDispatcher:boost::noncopyable
{
public:

    typedef boost::function<void (muduo::net::TcpConnectionPtr const&,
                                  MessagePtr const&,
                                  muduo::Timestamp)> ProtobufMessageCallback;

    explicit ProtobufDispatcher(ProtobufMessageCallback const& cb):defaultCallback_(cb)
    {}

    void onProtobufMessage(muduo::net::TcpConnectionPtr const& conn,
                   MessagePtr const& msg,
                   muduo::Timestamp receiveTime)
    {
        CallbackMap::const_iterator it = Callbacks_.find( msg->GetDescriptor() );
        if(it != Callbacks_.end())
        {
            it->second->onMessage(conn , msg , receiveTime);
        }
        else
            defaultCallback_(conn , msg , receiveTime);
    }

    template<typename T>
    void registerCallback(typename CallbackT<T>::ConcreteCallback const& cb)
    {
        boost::shared_ptr<CallbackT<T> > pb(new CallbackT<T>(cb));
        Callbacks_[T::descriptor()] = pb;
    }
private:

    typedef std::map<google::protobuf::Descriptor const* , boost::shared_ptr<Callback> > CallbackMap;
    CallbackMap Callbacks_;

    ProtobufMessageCallback defaultCallback_;
};

#endif