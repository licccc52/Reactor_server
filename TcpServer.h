#pragma once
#include"EventLoop.h"
#include"Socket.h"
#include"Channel.h"
#include"Acceptor.h"
#include"Connection.h"
#include<map>  // 一个TcpServer里面 会有很多Connection, 用map管理connection

class Connection;
class Acceptor;
//网络服务类
class TcpServer{

private:
    EventLoop loop_;  //一个TcpSetver可以有多个事件循环, 现在是单线程, 暂时只用一个事件循环.
    Acceptor *acceptor_; //一个TcpServer只有一个Acceptor对象 acceptor = listenfd
    std::map<int, Connection*> conns_; //一个TcpServer有多个Connection对象, 存放在map容器中

public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

    void start_run();

    //处理新客户端连接请求
    void newconnection(Socket *clientsock);
    void closeconnection(Connection *conn); //关闭客户端的连接, 在Connection类中回调此函数
    void errorconnection(Connection *conn); //客户端的连接错误, 在Connection类中回调此函数
    void onmessage(Connection *conn, std::string message); //处理客户端的请求报文, 在Connection类中回调此函数
    void sendcomplete(Connection *conn); // 数据发送完成后, 在Connection类中回调此函数, 用来通知TcpServer数据已经发送完成
    void epolltimeout(EventLoop *loop); //epoll_wait()超时, 在EventLoop类中回调此函数

};