#include"TcpServer.h"
#include<iostream>
/*

class TcpServer{

private:
    EventLoop loop_;  //一个TcpSetver可以有多个事件循环, 现在是单线程, 暂时只用一个事件循环.


public:
    TcpServer(const std::string &ip, const uint16_t port);
    ~TcpServer();

};

*/

TcpServer::TcpServer(const std::string &ip, const uint16_t port):loop_(EventLoop())
{
    if(&loop_ == nullptr){
        std::cout << "! ----- In TcpServer::TcpServer , loop_ is null ---- ! " << std::endl;
    }
    acceptor_ = new Acceptor(&loop_, ip, port);
    acceptor_->setnewconnection(std::bind(&TcpServer::newconnection, this, std::placeholders::_1));
    //此处调用回调函数是为了 创建 Connection类, 在accept()函数执行之后, 需要创建客户端channel
}

TcpServer::~TcpServer()
{
    delete acceptor_;

    for(auto &aa : conns_){
        delete aa.second;
    }
}

//运行事件循环
void TcpServer::start_run(){
    loop_.run();
}


//处理新客户端连接请求
void  TcpServer::newconnection(Socket *clientsock){
    Connection *conn = new Connection(&loop_, clientsock); //此处新对象还未释放
    conn->setclosecallback(std::bind(&TcpServer::closeconnection, this, std::placeholders::_1)); //此处的占位符可以直接赋conn, 以为后来调用的时候是赋值的conn对象的this指针
    conn->seterrorcallback(std::bind(&TcpServer::errorconnection, this, std::placeholders::_1));//此处的占位符可以直接赋conn, 以为后来调用的时候是赋值的conn对象的this指针
    conn->setonmessagecallback(std::bind(&TcpServer::onmessage, this, std::placeholders::_1, std::placeholders::_2));
    printf ("TcpServer::newconnection :  accept client InetAddress Instance created (fd=%d,ip=%s,port=%d) ok.\n", clientsock->fd(), conn->ip().c_str(), conn->port());
    conns_[conn->fd()] = conn;
}

void TcpServer::closeconnection(Connection *conn) //关闭客户端的连接, 在Connection类中回调此函数
{
    std::cout << "回调函数 TcpServer::closecallback() " << std::endl;
    printf("client(eventfd=%d) disconnected. \n", conn->fd());
    //从map容器中删除
    conns_.erase(conn->fd());
}

void TcpServer::errorconnection(Connection *conn) //客户端的连接错误, 在Connection类中回调此函数
{
    std::cout << "回调函数 TcpServer::errorcallback() " << std::endl;
    printf("client(eventfd=%d) error\n", conn->fd());
    //从map容器中删除
    conns_.erase(conn->fd());
}


//处理客户端的请求报文, 在Connection类中回调此函数
void TcpServer::onmessage(Connection *conn, std::string message)
{
    message="reply:"+message;
    
    int len=message.size();                        // 计算回应报文的大小。
    std::string tmpbuf((char*)&len,4);  // 把报文头部填充到回应报文中。
    tmpbuf.append(message);             // 把报文内容填充到回应报文中。
    
    send(conn->fd(),tmpbuf.data(),tmpbuf.size(),0);   // 把临时缓冲区中的数据直接send()出去。
}