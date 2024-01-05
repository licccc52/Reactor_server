#include"TcpServer.h"

/*

class TcpServer{

private:
    EventLoop loop_;  //一个TcpSetver可以有多个事件循环, 现在是单线程, 暂时只用一个事件循环.


public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

};

*/

TcpServer::TcpServer(const std::string &ip, const uint16_t port)
{
    Socket *servsock = new Socket(createnonblocking());
    InetAddress servaddr(ip, port);
    servsock->setkeepalive(true);
    servsock->setreuseaddr(true);
    servsock->settcpnodelay(true);
    servsock->setreuseport(true);

    servsock->bind(servaddr);
    servsock->listen();


    Channel *servchannel=new Channel(&loop_,servsock->fd());
    servchannel->setreadcallback(std::bind(&Channel::newconnection, servchannel, servsock));
    servchannel->enablereading(); //让epoll_wait()监视servchannel的读事件
}

TcpServer::~TcpServer()
{

}

//运行事件循环
void TcpServer::start_run(){
    loop_.run();
}