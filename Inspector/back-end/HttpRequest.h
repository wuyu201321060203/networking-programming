#ifndef HTTP_HTTPREQUEST_H
#define HTTP_HTTPREQUEST_H

#include <map>
#include <assert.h>
#include <stdio.h>
#include <type_traits>

#include <muduo/base/copyable.h>
#include <muduo/base/Timestamp.h>
#include <muduo/base/Types.h>
#include <muduo/base/Logging.h>

namespace muduo
{
namespace net
{
namespace detail
{

template <typename Enumeration>//g++ complier is OK
auto as_integer(Enumeration const value)
->typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

}

using muduo::net::detail::as_integer;
class HttpRequest : public muduo::copyable
{
public:

    typedef enum class Method
    {
        kInvalid = 0,
        kGet,
        kDelete,
        kPut,
        kPost,

    }Method;

    typedef enum class Version
    {
        kUnknown = 0,
        kHttp1dot0,
        kHttp1dot1,

    }Version;


    explicit HttpRequest():method_(Method::kInvalid) , version_(Version::kUnknown)
    {}

    void setVersion(Version const& version)
    {
        version_ = version;
    }

    bool setMethod(string const& method)
    {
        assert(method_ == Method::kInvalid);

        if(method == "GET")
            method_ = Method::kGet;

        else if(method == "DELETE")
            method_ = Method::kDelete;

        else if(method == "PUT")
            method_ = Method::kPut;

        else if(method == "POST")
            method_ = Method::kPost;

        else
            method_ = Method::kInvalid;

        return method_ != Method::kInvalid;
    }

    string const getMethodAsString() const
    {
        if(method_ == Method::kGet)
            return string ("GET");

        else if(method_ == Method::kDelete)
            return string ("DELETE");

        else if(method_ ==Method::kPut)
            return string ("PUT");

        else if(method_ == Method::kPost)
            return string ("Post");

        else
            return string ("Invalid");
    }

    Method getMethod() const
    {
        return method_;
    }

    void setPath(string const&  path)
    {
        assert(path != "");
        path_ = path;
    }

    string getPath() const
    {
        return path_;
    }

    void setQuery(string const& query)
    {
        query_ = query;
    }

    string getQuery() const
    {
        return query_;
    }

    void setHeaders(string const& headerline)
    {
        size_t pos = headerline.find_first_of(":");

        if(pos == string::npos)
        {
            LOG_ERROR << "http header is not valid";
            assert(false);
        }

        string key(std::begin(headerline) , std::begin(headerline) + pos);
        while(headerline[++pos] == ' ' && pos <= headerline.size() ){}

        string value = headerline.substr(pos);
        if(value != "")
        {
            int valueSize = value.size();

            while(value[--valueSize] == ' ')
                value.resize(valueSize);

            headers_[key] = value;
        }
    }

    std::map<string , string> const& getHeaders() const
    {
        return headers_;
    }

    string getContentByHeader(string const& header) const
    {
        std::map<string , string>::const_iterator i = headers_.find(header);

        if(i != headers_.end())
            return i->second;
        else
            return string();
    }

    void setReceiveTime(Timestamp const& timestamp)
    {
        receiveTime_ = timestamp;
    }

    Timestamp getReceiveTime() const
    {
        return receiveTime_;
    }

    void setRequestBody(string const& body)
    {
        requestBody_ = body;
    }

    string getRequestBody() const
    {
        return requestBody_;
    }

    void swap(HttpRequest& rhs)
    {
        std::swap(this->method_ , rhs.method_);
        path_.swap(rhs.path_);
        query_.swap(rhs.query_);
        headers_.swap(rhs.headers_);
        receiveTime_.swap(rhs.receiveTime_);
        requestBody_.swap(rhs.requestBody_);
    }

private:

    Method method_;
    string path_;
    string query_;
    Version version_;
    std::map<string , string> headers_;
    Timestamp receiveTime_;
    string requestBody_;
};
}
}

#endif