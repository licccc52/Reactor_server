#include"Acceptor.h"
#include<iostream>

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port):loop_(loop)
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
    
    acceptchannel_=new Channel(loop_,servsock_->fd());
    acceptchannel_->setreadcallback(std::bind(&Acceptor::newconnection, this));
    acceptchannel_->enablereading(); //让epoll_wait()监视servchannel的读事件
}



Acceptor::~Acceptor()
{
    delete servsock_;
    delete acceptchannel_;
}


// 处理新客户端连接请求
void Acceptor::newconnection() 
{
    ////////////////////////////////////////////////////////////////////////                    
    InetAddress clientaddr;// 注意，clientsock只能new出来，不能在栈上，否则析构函数会关闭fd。
    Socket *clientsock = new Socket(servsock_->accept(clientaddr));
    printf ("Channel::newconnection :  accept client InetAddress Instance created (fd=%d,ip=%s,port=%d) ok.\n", clientsock->fd(),clientaddr.ip(),clientaddr.port());

    /*
    // 为新客户端连接准备读事件，并添加到epoll中。
    Channel* clientchannel = new Channel(loop_, clientsock->fd());
    clientchannel->setreadcallback(std::bind(&Channel::onmessage,clientchannel));
    clientchannel->useet();
    clientchannel->enablereading();
    //客户端连接上来的fd采用边缘触发
    ////////////////////////////////////////////////////////////////////////
    */

//    Connection *conn = new Connection(loop_, clientsock);
    newconnectioncb_(clientsock);
    
}


void Acceptor::setnewconnection(std::function<void(Socket*)> fn)
{
    newconnectioncb_ = fn;
}
