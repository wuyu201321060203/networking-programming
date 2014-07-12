#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string>

#include <muduo/base/Types.h>
#include <muduo/base/FileUtil.h>

#include <boost/tokenizer.hpp>

#include "CpuUtilizationInspector.h"

using namespace muduo;
using namespace muduo::net;

static unsigned long g_cpuTotalTime = 0;
static unsigned long g_processCpuTime = 0;

void CpuUtilizationInspector::registerCommands(Inspector* ins)
{
    ins->add("performance", "cpu",
              CpuUtilizationInspector::getCpuUtilization,"get cpu utilization");
}

string CpuUtilizationInspector::getCpuUtilization(HttpRequest::Method
    , const Inspector::ArgList&)
{
    unsigned long cpuTotalTime = g_cpuTotalTime;
    unsigned long processCpuTime = g_processCpuTime;

    g_cpuTotalTime = 0;
    g_processCpuTime = 0;

    getCpuStatus();
    double utilization = static_cast<double>(g_processCpuTime - processCpuTime)
                        /(g_cpuTotalTime - cpuTotalTime);

    if(utilization > 1)
        utilization = 1;

    char buf[32];
    int ret = snprintf(buf , sizeof(buf) , "%lf%%" , utilization*100);
    assert(ret < 32);
    return string(buf , ret);
}


void CpuUtilizationInspector::getCommonCpuInfo(char const* filePath ,
                                               unsigned long& numericValue)
{
    std::string result;
    FileUtil::readFile(filePath, 65536, &result);
    assert(!result.empty());
    int counter = 0;
    boost::tokenizer<> tok(result);
    for(boost::tokenizer<>::iterator beg=tok.begin() ; beg!=tok.end() ; ++beg)
    {
        if(counter == 13 || counter == 14)
            numericValue += atoi( (*beg).c_str() );
        if(counter > 14)
            break;
        ++counter;
    }
}

void CpuUtilizationInspector::getCpuStatus()
{
    std::string procStatFirstLine;
    FileUtil::readFile("/proc/stat", 65536, &procStatFirstLine);
    assert(!procStatFirstLine.empty());
    size_t pos = procStatFirstLine.find_first_of("\n");
    assert(pos);
    std::string s = procStatFirstLine.substr(0 , pos);
    boost::tokenizer<> tok(s);
    for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
    {
        if( beg != tok.begin() )
            g_cpuTotalTime += atoi( (*beg).c_str() );
    }

    getCommonCpuInfo("/proc/self/stat" , g_processCpuTime);
}
