#include"Acceptor.h"
#include<iostream>

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port)
{
    if(loop == nullptr){
        std::cout << "! ----- In Acceptor::Acceptor , loop is null ---- ! " << std::endl;
    }
    std::cout << __FILE__ << " , "<< __LINE__ << ",   TcpServer Constructor" << std::endl;
    servsock_ = new Socket(createnonblocking());
    InetAddress servaddr(ip, port);
    servsock_->setkeepalive(true);
    servsock_->setreuseaddr(true);
    servsock_->settcpnodelay(true);
    servsock_->setreuseport(true);

    servsock_->bind(servaddr);
    servsock_->listen();
    
    loop_ = loop;
    acceptchannel_=new Channel(loop_,servsock_->fd());
    acceptchannel_->setreadcallback(std::bind(&Channel::newconnection, acceptchannel_, servsock_));
    acceptchannel_->enablereading(); //让epoll_wait()监视servchannel的读事件
}



Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}