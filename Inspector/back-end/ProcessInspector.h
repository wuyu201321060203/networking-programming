#ifndef PROCESSINSPECTOR_H
#define PROCESSINSPECTOR_H

#include <boost/noncopyable.hpp>

#include "Inspector.h"

namespace muduo
{
namespace net
{

class ProcessInspector : boost::noncopyable
{
 public:
  void registerCommands(Inspector* ins);

  static string overview(HttpRequest::Method, const Inspector::ArgList&);
  static string pid(HttpRequest::Method, const Inspector::ArgList&);
  static string procStatus(HttpRequest::Method, const Inspector::ArgList&);
  static string openedFiles(HttpRequest::Method, const Inspector::ArgList&);
  static string threads(HttpRequest::Method, const Inspector::ArgList&);

  static string username_;
};

}
}

#endif
