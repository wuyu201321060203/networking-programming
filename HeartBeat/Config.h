#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

#include <boost/weak_ptr.hpp>

#include <muduo/net/TcpConnection.h>

#include <google/protobuf/message.h>

typedef boost::weak_ptr<muduo::net::TcpConnection> TcpConnectionWeakPtr;
typedef std::vector<TcpConnectionWeakPtr> TcpConnectionWeakPtrVec;
typedef boost::shared_ptr<google::protobuf::Message> MessagePtr;

/* The macro for most used return value */
#define RET_SUCCESS 0
#define RET_FAIL -1

/* Database URL */

#define DB_URL "mysql://root:123@localhost:3306/DM"

/* The default number of threads */
#define DEFAULT_THREADS 3

/* The num of items of token*/
#define TOKEN_ITEM_NUM 5

typedef std::string STDSTR;
typedef unsigned long ulong;

typedef enum Role
{
    Master = 0,
    Slave
}Role;

#endif