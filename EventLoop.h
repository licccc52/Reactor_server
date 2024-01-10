#pragma once
#include<functional>
#include "Epoll.h"
#include "Channel.h"

//事件循环类
//Epoll 和 Channel, 负责 事件运行 
class Epoll;
class Channel;

class EventLoop
{
private:
    
    Epoll *ep_;     //每个事件循环只有一个Epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;//epoll_wait()超时的回调函数

public:

    EventLoop();    //在析构函数中创建Epoll对象ep_.
    ~EventLoop();   //在析构函数中销毁ep_

    void run(); // 运行事件循环
    Epoll* ep(); //返回Epoll对象
    void updatechannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);  //设置epoll_wait()超时的回调函数
};