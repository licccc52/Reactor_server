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

    for (int ii=0;ii<200000;ii++)
    {
        // 从命令行输入内容。
        memset(buf,0,sizeof(buf)); //初始化buffer, 发送数据缓冲区
        printf("please input:"); scanf("%s",buf);

        if (send(sockfd,buf,strlen(buf),0) <=0)       // 把命令行输入的内容发送给服务端。
        {  //send()函数 如果函数拷贝数据成功, 返回实际拷贝的字节数, 对方调用关闭函数来主动关闭连接返回0, 如果函数在拷贝数据时候出现错误返回-1
            printf("write() failed.\n");  close(sockfd);  return -1;
        }
        
        memset(buf,0,sizeof(buf));
        if (recv(sockfd,buf,sizeof(buf),0) <=0)      // 接收服务端的回应。
        { 
            printf("read() failed.\n");  close(sockfd);  return -1; //套接字关闭返回0 , 发生错误返回-1
        }

        printf("recv:%s\n",buf);
    }

    // printf("结束时间：%d",time(0));
} 