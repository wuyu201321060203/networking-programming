#ifndef UTIL_H
#define UTIL_H

#include <string>

#include <muduo/base/Types.h>

typedef std::string STDSTR;

STDSTR MuduoStr2StdStr(muduo::string const& s);
muduo::string StdStr2MuduoStr(STDSTR const& s);

#endif