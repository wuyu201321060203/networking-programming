#include <string>
#include <vector>

#include <muduo/base/Types.h>
#include <muduo/base/FileUtil.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>

#include <boost/algorithm/string.hpp>

#include "BandwidthInspector.h"

using namespace muduo;
using namespace muduo::net;
using namespace boost::algorithm;

static Timestamp g_currentTime;
static unsigned long g_recvBytes = 0;
static unsigned long g_sendBytes = 0;

void BandwidthInspector::registerCommands(Inspector* ins)
{
    ins->add("performance", "network",
              BandwidthInspector::getBandwidth,
              "get network Recv/Send rate");
}

string BandwidthInspector::getBandwidth(HttpRequest::Method
    , const Inspector::ArgList&)
{

    unsigned long recvBytes = g_recvBytes;
    unsigned long sendBytes = g_sendBytes;
    struct Timestamp currentTime = g_currentTime;
    g_recvBytes = 0;
    g_sendBytes = 0;
    getNetStatus();
    double interval = timeDifference(g_currentTime , currentTime);
    double recvRate = static_cast<double>(g_recvBytes - recvBytes)/interval
                      /1000000;
    double sendRate = static_cast<double>(g_sendBytes - sendBytes)/interval
                      /1000000;

    char buf[65];
    int ret = snprintf(buf , sizeof(buf) , "%lf:%lf" , recvRate , sendRate);
    assert(ret < 65);
    return string(buf , ret);
}

bool BandwidthInspector::isdigit(std::string const& str)
{
    for(unsigned int i = 0 ; i != str.size() ; ++i)
    {
        if( !std::isdigit(str[i]) )
            return false;
    }
    return true;
}

void BandwidthInspector::getFixedBytes(std::string const& text , int const& linenum)
{
    std::vector<std::string> lineVec;
    split(lineVec, text, is_any_of("\n"));
    std::vector<std::string> strVec;
    split(strVec , lineVec[linenum - 1] , is_any_of(" "));
    std::vector<unsigned long> digtalVec;
    for(std::vector<std::string>::iterator it = strVec.begin() ;
        it != strVec.end() ; ++it)
    {
        if( isdigit(*it) )
            digtalVec.push_back( atoi( (*it).c_str() ));
    }
    short m = 0;
    short n = 2;
    short step = 0;
    for(std::vector<unsigned long>::iterator it = digtalVec.begin() ;
        it != digtalVec.end() ; ++it)
    {
        if( *it == 0 )
            continue;

        else if( *it != 0 && step == m )
            g_recvBytes += *it;

        else if ( *it != 0 && step == n )
        {
            g_sendBytes += *it;
            break;
        }
        else
        {}
        ++step;
    }
}

void BandwidthInspector::getNetStatus()
{
    g_currentTime = Timestamp::now();
    std::string netDevStat;
    FileUtil::readFile("/proc/self/net/dev", 65536, &netDevStat);

    getFixedBytes(netDevStat , 3);
    getFixedBytes(netDevStat , 4);
}
