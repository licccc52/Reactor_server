#pragma once
#include "TcpServer.h"
#include "EventLoop.h"
#include "Connection.h"
#include "ThreadPool.h"

/*
    EchoServer类：回显服务器
*/
//此类相当于给TcpServer类套了一个壳子
class TcpServer;
class EchoServer
{
private:
    TcpServer tcpserver_;
    ThreadPool threadpool_;  //工作线程池

public:
    EchoServer(const std::string &ip,const uint16_t port, int subthreadnum = 3, int workthreadnum = 5);
    ~EchoServer();

    void Start();        // 启动服务。

    void HandleNewConnection(Connection *conn);     // 处理新客户端连接请求，在TcpServer类中回调此函数。
    void HandleClose(Connection *conn);                      // 关闭客户端的连接，在TcpServer类中回调此函数。 
    void HandleError(Connection *conn);                       // 客户端的连接错误，在TcpServer类中回调此函数。
    void HandleMessage(Connection *conn,std::string &message);     // 处理客户端的请求报文，在TcpServer类中回调此函数。
    void HandleSendComplete(Connection *conn);        // 数据发送完成后，在TcpServer类中回调此函数。
    // void HandleTimeOut(EventLoop *loop);                    // epoll_wait()超时，在TcpServer类中回调此函数。

    void OnMessage(Connection *conn, std::string &message);  //处理客户端的请求报文, 用于添加给线程池
};