#ifndef PROTOBUF_CODEC_H
#define PROTOBUF_CODEC_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <google/protobuf/message.h>

#include <muduo/net/Buffer.h>
#include <muduo/net/TcpConnection.h>

// struct ProtobufTransportFormat __attribute__ ((__packed__))
// {
//   int32_t  len;
//   int32_t  nameLen;
//   char     typeName[nameLen];
//   char     protobufData[len-nameLen-8];
//   int32_t  checkSum; // adler32 of nameLen, typeName and protobufData
// }

typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

class ProtobufCodec : boost::noncopyable
{
public:

    enum ErrorCode
    {
        kNoError = 0,
        kInvalidLength,
        kCheckSumError,
        kInvalidNameLen,
        kUnknownMessageType,
        kParseError,
    };

    typedef boost::function<void (muduo::net::TcpConnectionPtr const&,
        MessagePtr const&,
        muduo::Timestamp)> ProtobufMessageCallback;

    typedef boost::function<void (muduo::net::TcpConnectionPtr const&,
        muduo::net::Buffer*,
        muduo::Timestamp,
        ErrorCode)> ErrorCallback;

    explicit ProtobufCodec(ProtobufMessageCallback const& messageCb)
        : messageCallback_(messageCb),
        errorCallback_(defaultErrorCallback)
    {
    }

    ProtobufCodec(ProtobufMessageCallback const& messageCb, ErrorCallback const& errorCb)
        : messageCallback_(messageCb),
        errorCallback_(errorCb)
    {
    }

    void onMessage(muduo::net::TcpConnectionPtr const& conn,
        muduo::net::Buffer* buf,
        muduo::Timestamp receiveTime);

    void send(muduo::net::TcpConnectionPtr const& conn,
        google::protobuf::Message const& message)
    {
        muduo::net::Buffer buf;
        fillEmptyBuffer(&buf, message);
        conn->send(&buf);
    }

    static muduo::string const& errorCodeToString(ErrorCode errorCode);
    static void fillEmptyBuffer(muduo::net::Buffer* buf,
        google::protobuf::Message const& message);
    static google::protobuf::Message* createMessage(std::string const& type_name);
    static MessagePtr parse(char const* buf, int len, ErrorCode* errorCode);

private:

    static void defaultErrorCallback(muduo::net::TcpConnectionPtr const&,
        muduo::net::Buffer*,
        muduo::Timestamp,
        ErrorCode);

    ProtobufMessageCallback messageCallback_;
    ErrorCallback errorCallback_;

    static int const kHeaderLen = sizeof(int32_t);
    static int const kMinMessageLen = 2*kHeaderLen + 2;
    static int const kMaxMessageLen = 64*1024*1024;
};

#endif
