#include "TcpServer.h"

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.1.4 5050\n\n"); 
        return -1; 
    }

    TcpServer tcpserver(argv[1], atoi(argv[2]));
    
    tcpserver.start_run(); //运行事件循环
    
    return 0;
}