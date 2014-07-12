#ifndef BANDWIDTHINSPECTOR_H
#define BANDWIDTHINSPECTOR_H

#include <boost/noncopyable.hpp>

#include "Inspector.h"

namespace muduo
{
namespace net
{

class BandwidthInspector : boost::noncopyable
{
public:
    void registerCommands(Inspector* ins);

    static string getBandwidth(HttpRequest::Method , const Inspector::ArgList&);

    static void getNetStatus();

private:
    static void getFixedBytes(std::string const& , int const&);
    static bool isdigit(std::string const&);
};

}
}

#endif