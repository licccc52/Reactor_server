#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>          
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>      // TCP_NODELAY需要包含这个头文件。
#include"InetAddress.h"
#include"Socket.h"
#include"Epoll.h"
#include"Channel.h"

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.1.4 5050\n\n"); 
        return -1; 
    }

    Socket servsock(createnonblocking());
    servsock.setkeepalive(true);
    servsock.setreuseaddr(true);
    servsock.settcpnodelay(true);
    servsock.setreuseport(true);
    InetAddress servaddr(argv[1], atoi(argv[2]));
    servsock.bind(servaddr);
    servsock.listen();

    Epoll ep;
    Channel* servchannel = new Channel(&ep, servsock.fd(), true);
    servchannel->enablereading();
    

    while (true)        // 事件循环。
    {
        std::vector<Channel*> channels = ep.loop(); // 存放epoll_wait() 返回事件

        // 如果infds>0，表示有事件发生的fd的数量。
        for (auto &ch : channels)       // 遍历epoll返回的数组evs。
        {
            ch->handleevent(&servsock);
        }
    }

  return 0;
}