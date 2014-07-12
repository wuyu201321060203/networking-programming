#ifndef PERFORMANCEINSPECTOR_H
#define PERFORMANCEINSPECTOR_H

#include <boost/noncopyable.hpp>

#include "Inspector.h"

namespace muduo
{
namespace net
{

class PerformanceInspector : boost::noncopyable
{
 public:
  void registerCommands(Inspector* ins);

  static string heap(HttpRequest::Method, const Inspector::ArgList&);
  static string growth(HttpRequest::Method, const Inspector::ArgList&);
  static string profile(HttpRequest::Method, const Inspector::ArgList&);
  static string cmdline(HttpRequest::Method, const Inspector::ArgList&);
  static string memstats(HttpRequest::Method, const Inspector::ArgList&);
  static string memhistogram(HttpRequest::Method, const Inspector::ArgList&);

  static string symbol(HttpRequest::Method, const Inspector::ArgList&);
};

}
}

#endif
