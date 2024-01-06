#pragma once
#include<sys/epoll.h>
#include <functional>
#include"Epoll.h"
#include"InetAddress.h"
#include"Socket.h"
#include"EventLoop.h"


//Channel(通道), 封装了监听fd和客户端连接的fd

//监听的fd与客户端连接的fd的功能是不同的
//监听的fd的生命周期 -> 与服务器端程序相同 -> 封装成Accoptor类
//客户端连接的fd 的生命周期 -> 断开即终止  -> 封装成Connection类


class EventLoop;

class Channel{
private:
    int fd_ = -1;           //Channel拥有的fd, Channel和fd是一对一的关系
    // Epoll *ep_ = nullptr;   //Channel对应的红黑树, Channel和Epoll是多对一的关系, 一个Channel只对应一个Epoll
    EventLoop* loop_ = nullptr;
    bool inepoll_ = false;  //Channel是否已经添加到epoll上, 如果未添加, 调用epoll_ctl()的时候用EPOLL_CTL_ADD, 否则使用EPOLL_CTL_MOD
    uint32_t events_ = 0;   // fd_需要监视的事件, listenfd和clientfd需要监视的EPOLLIN, clientfd还可能需要监视EPOLLOUT
    uint32_t revents_ = 0;  // fd_已经发生的事件.
    std::function<void()> readcallback_ ; // fd_读事件的回调函数


public:
    Channel(EventLoop* loop, int fd_); // 构造函数
    ~Channel(); // 析构函数

    int fd();   //返回fd_成员
    void useet(); // 采用边缘触发
    void enablereading(); // 让epoll_wait()监视fd_的读事件
    void setinepoll(); // 把inepoll_成员的值设置为true
    void setrevents(uint32_t ev); //设置revents_成员的值 为参数ev
    bool inepoll(); //返回inepoll_成员
    uint32_t events(); // 返回events_成员
    uint32_t revents(); // 返回revents_成员

    void handleevent(); //事件处理函数, epoll_wait() 返回的之后执行它

    void newconnection(Socket* servsock); // 处理新客户端连接请求
    void onmessage(); // 处理对端发送过来的信息
    void setreadcallback(std::function<void()> fn);
};