#pragma once
#include<functional>
#include"Socket.h"
#include"InetAddress.h"
#include"Channel.h"
#include"EventLoop.h"

class EventLoop;
class Socket;
class Channel;


class Connection{

private:
    EventLoop * loop_;          //Accoptor对应的事件循环, 由形参传入,  一个Acceptor对应一个事件循环
    Socket *clientsock_;          //与客户端通信的Socket
    Channel *clientchannel_;     //Acceptor对应的channel, 在构造函数中创建


public:

    Connection(EventLoop *loop, Socket *clientsock);
    ~Connection();


};