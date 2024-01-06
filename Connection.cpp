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
    clientchannel_->useet();                //客户端练上来的fd采用边缘触发
    clientchannel_->enablereading();        //让epoll_wait()监视clientchannel的读事件
    
}


Connection::~Connection()
{
    delete clientchannel_;
    delete clientsock_;
}