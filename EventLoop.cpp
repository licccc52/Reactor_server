#include"EventLoop.h"
#include<iostream>
/*
//事件循环类
class EventLoop
{
private:
    
    Epoll *ep_;     //每个事件循环只有一个Epoll

public:

    EventLoop();    //在析构函数中创建Epoll对象ep_.
    ~EventLoop();   //在析构函数中销毁ep_

    void run(); // 运行事件循环
};

*/


EventLoop::EventLoop() :ep_(new Epoll)    //在析构函数中创建Epoll对象ep_.
{
    // std::cout << __FILE__ << " , "<< __LINE__ << ",   EventLoop Constructor" << std::endl;
}


EventLoop::~EventLoop()   //在析构函数中销毁ep_
{
    delete ep_;
}

#include<unistd.h>
#include<sys/syscall.h>

void EventLoop::run() // 运行事件循环
{
    // printf("EventLoop::run() thread is %ld.\n", syscall(SYS_gettid));
    while(true){//事件循环
        //超时事件设置为10s
        std::vector<Channel*> channels = ep_->loop(10 * 1000); // 存放epoll_wait() 返回事件,等待监视的fd有事件发生

        //如果channels为空, 表示超时, 回调TcpServer::connection
        if(channels.size() == 0){
            epolltimeoutcallback_(this); //在TcpServer中, EventLoop初始化的时候设置
        } 
        else{
            // 如果infds>0，表示有事件发生的fd的数量。
            for (auto &ch : channels)       // 遍历epoll返回的数组evs。
            {
                printf("In EventLoop::run() -> EVENTLOOP\n");
                ch->handleevent();
            }
        }
    }
    
}

Epoll* EventLoop::ep(){
    return ep_;
}

// 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
void EventLoop::updatechannel(Channel *ch)                        
{
    ep_->updatechannel(ch);
    
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn)  //设置epoll_wait()超时的回调函数
{
    epolltimeoutcallback_ = fn;
}