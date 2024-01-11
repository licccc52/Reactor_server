#pragma once
#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"Acceptor.h"
#include"Connection.h"
#include<map>  // 一个TcpServer里面 会有很多Connection, 用map管理connection
#include"ThreadPool.h"

class Connection;
class Acceptor;
//网络服务类
class TcpServer{

private:
    EventLoop *mainloop_;  //主事件循环, 使用堆内存(有使用者自己申请, 不用操作系统分配)
    std::vector<EventLoop*> subloops_; //存放从事件循环的容器
    Acceptor *acceptor_; //一个TcpServer只有一个Acceptor对象 acceptor = listenfd

    ThreadPool *threadpool_; //线程池
    int threadnum_;          //线程池大小, 从时间循环的个数

    std::map<int, Connection*> conns_; //一个TcpServer有多个Connection对象, 存放在map容器中
    std::function<void(Connection*)> newconnectioncb_;          // 回调EchoServer::HandleNewConnection()。
    std::function<void(Connection*)> closeconnectioncb_;        // 回调EchoServer::HandleClose()。
    std::function<void(Connection*)> errorconnectioncb_;         // 回调EchoServer::HandleError()。
    std::function<void(Connection*,std::string &message)> onmessagecb_;        // 回调EchoServer::HandleMessage()。
    std::function<void(Connection*)> sendcompletecb_;            // 回调EchoServer::HandleSendComplete()。
    std::function<void(EventLoop*)>  timeoutcb_;                       // 回调EchoServer::HandleTimeOut()。

public:
    TcpServer(const std::string &ip, const uint16_t port, int threadnumm = 3);
    ~TcpServer();

    void start_run();

    //处理新客户端连接请求
    void newconnection(Socket *clientsock);
    void closeconnection(Connection *conn); //关闭客户端的连接, 在Connection类中回调此函数
    void errorconnection(Connection *conn); //客户端的连接错误, 在Connection类中回调此函数
    void onmessage(Connection *conn, std::string &message); //处理客户端的请求报文, 在Connection类中回调此函数
    void sendcomplete(Connection *conn); // 数据发送完成后, 在Connection类中回调此函数, 用来通知TcpServer数据已经发送完成
    void epolltimeout(EventLoop *loop); //epoll_wait()超时, 在EventLoop类中回调此函数

    void setnewconnectioncb(std::function<void(Connection*)> fn);
    void setcloseconnectioncb(std::function<void(Connection*)> fn);
    void seterrorconnectioncb(std::function<void(Connection*)> fn);
    void setonmessagecb(std::function<void(Connection*,std::string &message)> fn);
    void setsendcompletecb(std::function<void(Connection*)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
};