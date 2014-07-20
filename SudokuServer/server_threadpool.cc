#include <utility>
#include <stdio.h>
#include <unistd.h>

#include <boost/bind.hpp>

#include <muduo/base/Atomic.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/base/ThreadPool.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Singleton.h>

#include "sudoku.h"
#include "TaskFactory.h"
#include "TaskManager.h"

using namespace muduo;
using namespace muduo::net;

typedef muduo::Singleton<TaskFactory> SingleTaskFactory;
typedef muduo::Singleton<TaskManager> SingleTaskManager;

class SudokuServer
{
 public:
  SudokuServer(EventLoop* loop, InetAddress const& listenAddr, int numThreads)
    : server_(loop, listenAddr, "SudokuServer"),
      numThreads_(numThreads),
      startTime_(Timestamp::now())
  {
    server_.setConnectionCallback(
        boost::bind(&SudokuServer::onConnection, this, _1));
    server_.setMessageCallback(
        boost::bind(&SudokuServer::onMessage, this, _1, _2, _3));
  }

  void start()
  {
    LOG_INFO << "starting " << numThreads_ << " threads.";
    SingleTaskManager::instance().startThreadPool(numThreads_);
    server_.start();
  }

 private:
  void onConnection(TcpConnectionPtr const& conn)
  {
    LOG_TRACE << conn->peerAddress().toIpPort() << " -> "
        << conn->localAddress().toIpPort() << " is "
        << (conn->connected() ? "UP" : "DOWN");
  }

  void onMessage(TcpConnectionPtr const& conn, Buffer* buf, Timestamp)
  {
    LOG_DEBUG << conn->name();
    size_t len = buf->readableBytes();
    while (len >= kCells + 2)
    {
      char const* crlf = buf->findCRLF();
      if (crlf)
      {
        string request(buf->peek(), crlf);
        buf->retrieveUntil(crlf + 2);
        len = buf->readableBytes();
        if (!processRequest(conn, request))
        {
          conn->send("Bad Request!\r\n");
          conn->shutdown();
          break;
        }
      }
      else if (len > 100) // id + ":" + kCells + "\r\n"
      {
        conn->send("Id too long!\r\n");
        conn->shutdown();
        break;
      }
      else
      {
        break;
      }
    }
  }

  bool processRequest(TcpConnectionPtr const& conn, string const& request)
  {
    string id;
    string puzzle;
    bool goodRequest = true;

    string::const_iterator colon = find(request.begin(), request.end(), ':');
    if (colon != request.end())
    {
      id.assign(request.begin(), colon);
      puzzle.assign(colon+1, request.end());
    }
    else
    {
      puzzle = request;
    }

    if (puzzle.size() == implicit_cast<size_t>(kCells))
    {
      TaskPtr task = SingleTaskFactory::instance().createTask(boost::bind(&solve,
                                                              conn, puzzle, id));
      SingleTaskManager::instance().runTask(task);
    }
    else
    {
      goodRequest = false;
    }
    return goodRequest;
  }

  static void solve(TcpConnectionPtr const& conn,
                    string const& puzzle,
                    string const& id)
  {
    LOG_DEBUG << conn->name();
    string result = solveSudoku(puzzle);
    if (id.empty())
    {
      conn->send(result+"\r\n");
    }
    else
    {
      conn->send(id+":"+result+"\r\n");
    }
  }

  TcpServer server_;
  int numThreads_;
  Timestamp startTime_;
};

int main(int argc, char* argv[])
{
  LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
  int numThreads = 0;
  if (argc > 1)
  {
    numThreads = atoi(argv[1]);
  }
  EventLoop loop;
  InetAddress listenAddr(9981);
  SudokuServer server(&loop, listenAddr, numThreads);

  server.start();

  loop.loop();
}
