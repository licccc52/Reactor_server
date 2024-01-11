#pragma once
#include<functional>
#include"Socket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"
#include"Buffer.h"

class EventLoop;
class Socket;
class Channel;
class Buffer; 

class Connection{

private:
    EventLoop * loop_;          //Accoptor对应的事件循环, 由形参传入,  一个Acceptor对应一个事件循环
    Socket *clientsock_;          //与客户端通信的Socket
    Channel *clientchannel_;     //Acceptor对应的channel, 在构造函数中创建
    std::function<void(Connection*)> closecallback_; //关闭fd_的回调函数, 将回调TcpServer::closeConnection()
    std::function<void(Connection*)> errorcallback_; //连接错误的fd_的回调函数, 将回调TcpServer::errorConnection()
    std::function<void(Connection*, std::string&)> onmessagecallback_; //处理报文的回调函数, 将回调TcpServer::onmessage()
    std::function<void(Connection*)> sendcompletecallback_; // connection发送信息完成之后, 回调此函数

    

    Buffer inputbuffer_;         //接收缓冲区
    Buffer outputbuffer_;         //发送缓冲区


public:

    Connection(EventLoop *loop, Socket *clientsock);
    ~Connection();


    int fd() const;              // 返回fd_成员。
    std::string ip() const;     //返回fd_成员
    uint16_t port() const;      //返回port_成员

    void onmessage(); // 处理对端发送过来的信息

    void closecallback();       //TCP连接关闭(断开)的回调函数, 供Channel回调
    void errorcallback();       //TCP连接错误的回调函数, 共Channel回调
    void writecallback();       //处理写事件的回调函数, 供Channel回调


    void setclosecallback(std::function<void(Connection*)> fn);
    void seterrorcallback(std::function<void(Connection*)> fn);
    void setonmessagecallback(std::function<void(Connection*, std::string&)> fn); //设置处理报文的回调函数
    void setsendcompletecallback(std::function<void(Connection*)> fn); //设置 当Connection类对象完成信息发送的时候回调的函数

    void send(const char *data, size_t size); //发送数据


};