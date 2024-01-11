#include "EchoServer.h"
class EchoServer;

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.1.4 5050\n\n"); 
        return -1; 
    }

    EchoServer server(argv[1], atoi(argv[2]));
    
    server.Start(); //运行事件循环
    
    return 0;
}
//ps -ef|grep tcpepoll 先查进程ID
//ps -T -p TID 显示线程id