// 网络通讯的客户端程序。
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
// #include<iostream>

int main(int argc, char *argv[])//
{
    if (argc != 3)
    {
        printf("usage:./client ip port\n"); 
        printf("example:./client 192.168.1.4 5050\n\n"); 

        return -1;
    }

    int sockfd;
    struct sockaddr_in servaddr;
    char buf[1024];
 
    if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0) { printf("socket() failed.\n"); return -1; } //成功的话返回文件描述符, 保存在sockfd中  file description
    
    memset(&servaddr,0,sizeof(servaddr)); //初始化struct sockaddr_in servaddr
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(atoi(argv[2])); // atoi() 把一个string 转换为 integer 
    //The htons() function converts the unsigned short integer hostshort from host byte order to network byte order. 主机字节序转换为网络字节序, 用户输入的端口号
    servaddr.sin_addr.s_addr=inet_addr(argv[1]); /* 从 CP 中的数字和点符号转换 Internet 主机地址   按网络字节顺序转换为二进制数据。 */

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connect(%s:%s) failed.\n",argv[1],argv[2]); close(sockfd);  return -1;
    }

    printf("connect ok.\n");
    // printf("开始时间：%d",time(0));

    for (int ii=0;ii<10;ii++)
    {
        memset(buf,0,sizeof(buf));
        sprintf(buf,"这是第%d个超级女生。",ii);

        char tmpbuf[1024];                 // 临时的buffer，报文头部+报文内容。
        memset(tmpbuf,0,sizeof(tmpbuf));
        int len=strlen(buf);                 // 计算报文的大小。
        memcpy(tmpbuf,&len,4);       // 拼接报文头部。
        memcpy(tmpbuf+4,buf,len);  // 拼接报文内容。

        send(sockfd,tmpbuf,len+4,0);  // 把请求报文发送给服务端。
    }
        
    for (int ii=0;ii<10;ii++)
    {
        int len;
        recv(sockfd,&len,4,0);            // 先读取4字节的报文头部。

        memset(buf,0,sizeof(buf));
        recv(sockfd,buf,len,0);           // 读取报文内容。

        printf("recv:%s\n",buf);
    }

    // printf("结束时间：%d\n",time(0));
} 