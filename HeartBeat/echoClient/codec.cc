#include <string>
#include <zlib.h>  // adler32加密算法

#include <google/protobuf/descriptor.h>

#include <muduo/base/Logging.h>
#include <muduo/net/Endian.h>
#include <muduo/net/protorpc/google-inl.h>

#include "codec.h"

using namespace muduo;
using namespace muduo::net;

void ProtobufCodec::fillEmptyBuffer(Buffer* buf,
                                    google::protobuf::Message const& message)
{
    assert(buf->readableBytes() == 0);

    std::string const& typeName = message.GetTypeName();
    int32_t nameLen = static_cast<int32_t>(typeName.size()+1);
    buf->appendInt32(nameLen);
    buf->append(typeName.c_str(), nameLen);

    GOOGLE_DCHECK(message.IsInitialized()) <<
        InitializationErrorMessage("serialize", message);

    int byte_size = message.ByteSize();
    buf->ensureWritableBytes(byte_size);

    uint8_t* start = reinterpret_cast<uint8_t*>(buf->beginWrite());
    uint8_t* end = message.SerializeWithCachedSizesToArray(start);
    std::string tmp(reinterpret_cast<char*>(start) , byte_size);
    if (end - start != byte_size)
    {
        ByteSizeConsistencyError(byte_size, message.ByteSize(),
            static_cast<int>(end - start));
    }
    buf->hasWritten(byte_size);

    int32_t checkSum = static_cast<int32_t>(
        ::adler32(1,
        reinterpret_cast<Bytef const*>(buf->peek()),
        static_cast<int>(buf->readableBytes())));
    buf->appendInt32(checkSum);
    assert(buf->readableBytes() == sizeof nameLen + nameLen + byte_size +
        sizeof checkSum);
    int32_t len = sockets::hostToNetwork32(static_cast<int32_t>(buf->readableBytes()));
    buf->prepend(&len, sizeof len);
}

namespace
{
    string const kNoErrorStr = "NoError";
    string const kInvalidLengthStr = "InvalidLength";
    string const kCheckSumErrorStr = "CheckSumError";
    string const kInvalidNameLenStr = "InvalidNameLen";
    string const kUnknownMessageTypeStr = "UnknownMessageType";
    string const kParseErrorStr = "ParseError";
    string const kUnknownErrorStr = "UnknownError";
}

string const& ProtobufCodec::errorCodeToString(ErrorCode errorCode)
{
    switch (errorCode)
    {
    case kNoError:
        return kNoErrorStr;
    case kInvalidLength:
        return kInvalidLengthStr;
    case kCheckSumError:
        return kCheckSumErrorStr;
    case kInvalidNameLen:
        return kInvalidNameLenStr;
    case kUnknownMessageType:
        return kUnknownMessageTypeStr;
    case kParseError:
        return kParseErrorStr;
    default:
        return kUnknownErrorStr;
    }
}

void ProtobufCodec::defaultErrorCallback(muduo::net::TcpConnectionPtr const& conn,
                                         muduo::net::Buffer* buf,
                                         muduo::Timestamp,
                                         ErrorCode errorCode)
{
    LOG_ERROR << "ProtobufCodec::defaultErrorCallback - " <<
        errorCodeToString(errorCode);
    if (conn && conn->connected())
    {
        conn->shutdown();
    }
}

int32_t asInt32(char const* buf)
{
    int32_t be32 = 0;
    ::memcpy(&be32, buf, sizeof(be32));
    return sockets::networkToHost32(be32);
}

void ProtobufCodec::onMessage(TcpConnectionPtr const& conn,
                              Buffer* buf,
                              Timestamp receiveTime)
{
    while (buf->readableBytes() >= kMinMessageLen + kHeaderLen)
    {
        int32_t const len = buf->peekInt32();
        if (len > kMaxMessageLen || len < kMinMessageLen)
        {
            errorCallback_(conn, buf, receiveTime, kInvalidLength);
            break;
        }
        else if (buf->readableBytes() >= implicit_cast<size_t>(len + kHeaderLen))
        {
            ErrorCode errorCode = kNoError;
            MessagePtr message = parse(buf->peek()+kHeaderLen, len, &errorCode);
            if (errorCode == kNoError && message)
            {
                messageCallback_(conn, message, receiveTime);
                buf->retrieve(kHeaderLen+len);
            }
            else
            {
                errorCallback_(conn, buf, receiveTime, errorCode);
                break;
            }
        }
        else
        {
            break;
        }
    }
}

google::protobuf::Message* ProtobufCodec::createMessage(std::string const& typeName)
{
    google::protobuf::Message* message = NULL;
    google::protobuf::Descriptor const* descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (descriptor)
    {
        google::protobuf::Message const* prototype =
            google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype)
        {
            message = prototype->New();//原型模式
        }
    }
    return message;
}

MessagePtr ProtobufCodec::parse(char const* buf, int len, ErrorCode* error)
{
    MessagePtr message;

    int32_t expectedCheckSum = asInt32(buf + len - kHeaderLen);
    int32_t checkSum = static_cast<int32_t>(
        ::adler32(1,
        reinterpret_cast<Bytef const*>(buf),
        static_cast<int>(len - kHeaderLen)));
    if (checkSum == expectedCheckSum)
    {
        int32_t nameLen = asInt32(buf);
        if (nameLen >= 2 && nameLen <= len - 2*kHeaderLen)
        {
            std::string typeName(buf + kHeaderLen, buf + kHeaderLen + nameLen - 1);
            message.reset(createMessage(typeName));
            if (message)
            {
                char const* data = buf + kHeaderLen + nameLen;
                int32_t dataLen = len - nameLen - 2*kHeaderLen;
                std::string tmp(data , dataLen);
                if (message->ParseFromArray(data, dataLen))
                {
                    *error = kNoError;
                }
                else
                {
                    *error = kParseError;
                }
            }
            else
            {
                *error = kUnknownMessageType;
            }
        }
        else
        {
            *error = kInvalidNameLen;
        }
    }
    else
    {
        *error = kCheckSumError;
    }

    return message;
}
