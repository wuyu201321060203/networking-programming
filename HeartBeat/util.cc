#include "util.h"

STDSTR MuduoStr2StdStr(muduo::string const& s)
{
    return STDSTR(s.c_str());
}

muduo::string StdStr2MuduoStr(STDSTR const& s)
{
    return muduo::string(s.c_str());
}