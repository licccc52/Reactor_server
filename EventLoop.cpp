#include"EventLoop.h"
#include<iostream>

EventLoop::EventLoop() //在析构函数中创建Epoll对象ep_.
        :ep_(new Epoll), wakeupfd_(eventfd(0, EFD_NONBLOCK)),wakechannel_(new Channel(this, wakeupfd_))    
{
    // std::cout << __FILE__ << " , "<< __LINE__ << ",   EventLoop Constructor" << std::endl;
    wakechannel_->setreadcallback(std::bind(&EventLoop::handlewakeup, this));
    wakechannel_->enablereading(); //激活读事件, 如果事件循环被唤醒(IO循环), 就会激活 handlewakeup, 然后执行发送操作
}



EventLoop::~EventLoop()   //在析构函数中销毁ep_
{
    // delete ep_;
}


void EventLoop::run() // 运行事件循环
{
    // printf("EventLoop::run() thread is %ld.\n", syscall(SYS_gettid));
    threadid_ = syscall(SYS_gettid); //获取事件循环所在的id
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


// 把channel添加/更新到红黑树上，channel中有fd，也有需要监视的事件。
void EventLoop::updatechannel(Channel *ch)                        
{
    ep_->updatechannel(ch);
    
}

void EventLoop::removechannel(Channel *ch)                        // 从红黑树上删除Channel
{
    ep_->removechannel(ch);
}

void EventLoop::setepolltimeoutcallback(std::function<void(EventLoop*)> fn)  //设置epoll_wait()超时的回调函数
{
    epolltimeoutcallback_ = fn;
}

bool EventLoop::isinloopthread() // 判断当前线程是否为事件循环线程
{
    return threadid_ == syscall(SYS_gettid); 
}

//把任务添加到队列中
void EventLoop::queueinloop(std::function<void()> fn){
    {
        std::lock_guard<std::mutex> gd(mutex_); //给任务队列加锁
        taskqueue_.push(fn); // 任务入队
    }
    
    wakeup();//唤醒循环事件
}

void EventLoop::wakeup() //唤醒事件循环
{
    uint64_t val = 1;
    write(wakeupfd_, &val, sizeof(val));//随便写入一点数据唤醒线程
}

void EventLoop::handlewakeup() //事件循环线程被eventfd唤醒后执行的函数
{
    printf("EventLoop::handlewakeup(), thread id is %ld.\n", syscall(SYS_gettid));
    
    uint64_t val;
    read(wakeupfd_, &val, sizeof(val)); //从eventfd中读取出数据, 如果不读取, 在水平触发模式下eventfd的读事件会一直触发
    //wakeupfd_中没有数据写入的话, 函数就会被阻塞到这一步, 所以在sendinloop中调用wakeup才有效果
    std::function<void()> fn;
    
    std::lock_guard<std::mutex> gd(mutex_); //给任务队列加锁

    while(taskqueue_.size() > 0){
        fn = std::move(taskqueue_.front()); //出队一个元素
        taskqueue_.pop();
        fn();       //执行任务
    }
}