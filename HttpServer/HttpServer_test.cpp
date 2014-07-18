#include <iostream>
#include <map>

#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpServer.h"

using namespace muduo;
using namespace muduo::net;

extern char favicon[555];
bool benchmark = false;

void onRequest(HttpRequest const& req, HttpResponse& resp)
{
  //std::cout << "Headers " << req.methodString() << " " << req.path() << std::endl;
  if (!benchmark)
  {
    const std::map<string, string>& headers = req.getHeaders();
    for (std::map<string, string>::const_iterator it = headers.begin();
         it != headers.end();
         ++it)
    {
      std::cout << it->first << ": " << it->second << std::endl;
    }
  }

  if (req.getPath() == "/")
  {
    resp.setStatusCode(HttpResponse::HttpStatusCode::k200Ok);
    resp.setStatusMessage("OK");
    resp.setContentType("text/html");
    resp.setHeader("Server", "Muduo");
    string now = Timestamp::now().toFormattedString();
    resp.setBody("<html><head><title>This is title</title></head>"
        "<body><h1>Hello</h1>Now is " + now +
        "</body></html>");
  }
  else if (req.getPath() == "/hello")
  {
    resp.setStatusCode(HttpResponse::HttpStatusCode::k200Ok);
    resp.setStatusMessage("OK");
    resp.setContentType("text/plain");
    resp.setHeader("Server", "Muduo");
    resp.setBody("hello, world!\n");
  }
  else if (req.getPath() == "/angel")
  {
    resp.setStatusCode(HttpResponse::HttpStatusCode::k200Ok);
    resp.setStatusMessage("OK");
    resp.setContentType("text/plain");
    resp.setHeader("Server", "Muduo");
    resp.setBody("hello, angel!\n");
  }

  else
  {
    resp.setStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
    resp.setStatusMessage("Not Found");
    resp.setCloseConnection(true);
  }
}

int main(int argc, char* argv[])
{
  int numThreads = 0;
  if (argc > 1)
  {
    benchmark = true;
    Logger::setLogLevel(Logger::WARN);
    numThreads = atoi(argv[1]);
  }
  EventLoop loop;
  HttpServer server(&loop, InetAddress(12345), "dummy");
  server.setHttpCallback(onRequest);
  server.setThreadNum(numThreads);
  server.start();
  loop.loop();
}
