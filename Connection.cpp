#include"Connection.h"
#include<iostream>

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop), clientsock_(clientsock)
{
    if(loop == nullptr || clientsock == nullptr){
        std::cout << "! ----- In Connection::Connection , loop or clientsock is null ---- ! " << std::endl;
    }
    // 为新客户端连接准备读事件，并添加到epoll中。
    clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Channel::onmessage,clientchannel_));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    
    clientchannel_->useet();                //客户端练上来的fd采用边缘触发
    clientchannel_->enablereading();        //让epoll_wait()监视clientchannel的读事件
    
}


Connection::~Connection()
{
    delete clientchannel_;
    delete clientsock_;
}


int Connection::fd() const              // 返回fd_成员。
{
    return clientsock_->fd();
}

std::string Connection::ip() const     //返回fd_成员
{
    return clientsock_->ip();
}

uint16_t Connection::port() const      //返回port_成员
{
    return clientsock_->port();
}


void Connection::closecallback()       //TCP连接关闭(断开)的回调函数, 供Channel回调
{
    // std::cout << "回调函数 Connection::closecallback() " << std::endl;
    // printf("client(eventfd=%d) error.\n", fd());
    // close(fd());
    closecallback_(this);
}
void Connection::errorcallback()       //TCP连接错误的回调函数, 共Channel回调
{
    // std::cout << "回调函数 Connection::errorcallback() " << std::endl;
    // printf("client(eventfd=%d) error\n", fd());
    // close(fd());
    errorcallback_(this);
}

//设置关闭fd_的回调函数
void Connection::setclosecallback(std::function<void(Connection*)> fn)
{
    closecallback_ = fn;
}
void Connection::seterrorcallback(std::function<void(Connection*)> fn)
{
    errorcallback_ = fn;
}