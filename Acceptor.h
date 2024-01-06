#pragma once
#include<functional>
#include"Socket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"
#include"Connection.h"

class EventLoop;
class Socket;
class Channel;


class Acceptor{

private:
    EventLoop * loop_;          //Accoptor对应的事件循环, 由形参传入,  一个Acceptor对应一个事件循环
    Socket *servsock_;          //服务端用于监听的socket, 在构造函数中创建
    Channel *acceptchannel_;     //Acceptor对应的channel, 在构造函数中创建
    std::function<void(Socket*)> newconnectioncb_; 
    // 处理新客户端连接请求的回调函数, 将指向TcpServer::newconnection()


public:

    Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port);
    ~Acceptor();

    void newconnection(); // 处理新客户端连接请求

    void setnewconnection(std::function<void(Socket*)> fn);
};