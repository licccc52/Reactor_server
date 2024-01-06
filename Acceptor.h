#pragma once
#include<functional>
#include"Socket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"

class EventLoop;
class Socket;
class Channel;


class Acceptor{

private:
    EventLoop * loop_;          //Accoptor对应的事件循环, 由形参传入,  一个Acceptor对应一个事件循环
    Socket *servsock_;          //服务端用于监听的socket, 在构造函数中创建
    Channel *acceptchannel_;     //Acceptor对应的channel, 在构造函数中创建


public:

    Acceptor(EventLoop *loop, const std::string &ip, const uint16_t port);
    ~Acceptor();


};