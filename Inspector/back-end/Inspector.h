#ifndef INSPECT_INSPECTOR_H
#define INSPECT_INSPECTOR_H

#include <muduo/base/Mutex.h>

#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "HttpRequest.h"
#include "HttpServer.h"

namespace muduo
{
namespace net
{

class ProcessInspector;
class PerformanceInspector;
class CpuUtilizationInspector;
class BandwidthInspector;

// An internal inspector of the running process, usually a singleton.
// Better to run in a seperated thread, as some method may block for seconds
class Inspector : boost::noncopyable
{
public:
    typedef std::vector<string> ArgList;
    typedef boost::function<string (HttpRequest::Method, ArgList const& args)> Callback;
    Inspector(EventLoop* loop,
        InetAddress const& httpAddr,
        string const& name);
    ~Inspector();

    void add(string const& module,
        string const& command,
        Callback const& cb,
        string const& help);

private:
    typedef std::map<string, Callback> CommandList;
    typedef std::map<string, string> HelpList;

    void start();
    void onRequest(HttpRequest const& req, HttpResponse& resp);

    HttpServer server_;
    boost::scoped_ptr<ProcessInspector> processInspector_;
    boost::scoped_ptr<PerformanceInspector> performanceInspector_;
    boost::scoped_ptr<CpuUtilizationInspector> cpuInspector_;
    boost::scoped_ptr<BandwidthInspector> bandwidthInspector_;
    MutexLock mutex_;
    std::map<string, CommandList> modules_;
    std::map<string, HelpList> helps_;

};

}
}

#endif
