#ifndef CPUUTILIZATIONINSPECTOR_H
#define CPUUTILIZATIONINSPECTOR_H

#include <boost/noncopyable.hpp>

#include "Inspector.h"

namespace muduo
{
namespace net
{

class CpuUtilizationInspector : boost::noncopyable
{
public:
    void registerCommands(Inspector* ins);

    static string getCpuUtilization(HttpRequest::Method , const Inspector::ArgList&);

    static void getCpuStatus();

private:
    static void getCommonCpuInfo(char const* , unsigned long&);
};

}
}

#endif