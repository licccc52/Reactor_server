#pragma once
#include <functional>
#include "Epoll.h"
#include "Channel.h"
#include <memory>
#include <unistd.h>
#include <sys/syscall.h>
#include <queue> 
#include <mutex>
#include <sys/eventfd.h>

//事件循环类
//Epoll 和 Channel, 负责 事件运行 
class Epoll;
class Channel;

class EventLoop
{
private:    //事件循环类
    std::unique_ptr<Epoll> ep_;     //每个事件循环只有一个Epoll
    std::function<void(EventLoop*)> epolltimeoutcallback_;//epoll_wait()超时的回调函数, 此回调函数在TcpServer中初始化的时候设置
    pid_t threadid_; //事件循环所在线程的id
    std::queue<std::function<void()>> taskqueue_; //事件循环线程被eventfd唤醒后执行的任务队列
    std::mutex mutex_;  //任务队列同步的互斥锁
    int wakeupfd_;
    std::unique_ptr<Channel> wakechannel_; //eventfd的channel 

public:

    EventLoop();    //在析构函数中创建Epoll对象ep_.
    ~EventLoop();   //在析构函数中销毁ep_

    void run(); // 运行事件循环

    void updatechannel(Channel *ch);                        // 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
    void removechannel(Channel *ch);                        // 从红黑树上删除Channel
    void setepolltimeoutcallback(std::function<void(EventLoop*)> fn);  //设置epoll_wait()超时的回调函数

    bool isinloopthread(); // 判断当前线程是否为事件循环线程

    void queueinloop(std::function<void()> fn); //把任务添加到队列中
    void wakeup(); //唤醒事件循环
    void handlewakeup(); //事件循环线程被eventfd唤醒后执行的函数

};