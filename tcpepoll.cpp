/*
 * 程序名：tcpepoll.cpp，此程序用于演示采用epoll模型实现网络通讯的服务端。
 * 作者：吴从周
*/
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

// 设置非阻塞的IO。
void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}
//fcntl(fd, F_GETFL) 用于获取文件描述符 fd 的当前状态标志。
//fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) 将获取到的状态标志与 O_NONBLOCK 做按位或操作，然后将结果设置为文件描述符 fd 的新状态标志。O_NONBLOCK 是一个标志常量，用于表示非阻塞模式。
//这段代码的作用是将文件描述符 fd 的状态标志设置为原有标志的基础上加上 O_NONBLOCK 标志，以使得对该文件描述符的 I/O 操作变为非阻塞模式。
//在非阻塞模式下，I/O 操作会立即返回，如果没有数据可读或者无法立即进行写操作，函数会立即返回而不是等待。

int main(int argc,char *argv[])
{
    if (argc != 3) 
    { 
        printf("usage: ./tcpepoll ip port\n"); 
        printf("example: ./tcpepoll 192.168.1.4 5050\n\n"); 
        return -1; 
    }

    // 创建服务端用于监听的listenfd。
    int listenfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//IPPROTO_TCP表示使用TCP协议
    if (listenfd < 0)
    {
        perror("socket() failed"); return -1;
    }

    // 设置listenfd的属性，如果对这些属性不熟悉，百度之。
    int opt = 1; 
    /* Set socket FD's option OPTNAME at protocol level LEVEL
   to *OPTVAL (which is OPTLEN bytes long).
   Returns 0 on success, -1 for errors.  */

// extern int setsockopt (int __fd, int __level, int __optname, const void *__optval, socklen_t __optlen) __THROW;
// static_cast 是 C++ 中四个命名强制类型转换操作符之一, 用于执行各种不同类型之间的转换


// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
// sockfd: 是要设置选项的套接字描述符。
// level: 表示选项所在的协议层。常见的有 SOL_SOCKET（通用套接字选项）、IPPROTO_IP（IP 层选项）、IPPROTO_TCP（TCP 层选项）等。
// optname: 表示需要设置的选项名，比如 SO_REUSEADDR（允许地址重用）、SO_KEEPALIVE（启用连接保活）、IP_TTL（IP 数据包生存时间）等。
// optval: 是一个指向包含新选项值的缓冲区的指针。
// optlen: 是 optval 指向的缓冲区的大小。

    setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的, 允许在套接字关闭后立即重新使用先前使用过的端口, 对于服务器程序快速重启并绑定到相同端口来说很有用
    //用于任意类型, 任意状态套接口的 设置选项值
    setsockopt(listenfd,SOL_SOCKET,TCP_NODELAY   ,&opt,static_cast<socklen_t>(sizeof opt));    // 必须的, 禁用Nagle算法, 允许小包的发送, 对于延时敏感型,同时数量传输量比较小的应用很有用
    setsockopt(listenfd,SOL_SOCKET,SO_REUSEPORT ,&opt,static_cast<socklen_t>(sizeof opt));    // 有用，但是，在Reactor中意义不大。
    setsockopt(listenfd,SOL_SOCKET,SO_KEEPALIVE   ,&opt,static_cast<socklen_t>(sizeof opt));    // 可能有用，但是，建议自己做心跳。

    setnonblocking(listenfd);    // 把服务端的listenfd设置为非阻塞的。

    struct sockaddr_in servaddr;                                  // 服务端地址的结构体。
    servaddr.sin_family = AF_INET;                              // IPv4网络协议的套接字类型。
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);      // 服务端用于监听的ip地址。
    servaddr.sin_port = htons(atoi(argv[2]));               // 服务端用于监听的端口。

    if (bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0 )
    {
        perror("bind() failed"); close(listenfd); return -1;
    }

    if (listen(listenfd,128) != 0 )        // 在高并发的网络服务器中，第二个参数要大一些。
    {
        perror("listen() failed"); close(listenfd); return -1;
    }

    int epollfd=epoll_create(1);        // 创建epoll句柄（红黑树）。
    //句柄 : 是一个int值, 逻辑上相当于指针的指针, 本质上是对底层硬件实例的指针的引用, 用引用计数机制管理句柄的生命周期, 为了节约资源, 
    // 为服务端的listenfd准备读事件。
    struct epoll_event ev;            // 声明事件的数据结构。
    ev.data.fd=listenfd;              // 指定事件的自定义数据，会随着epoll_wait()返回的事件一并返回。
    ev.events=EPOLLIN;                // 让epoll监视listenfd的读事件，采用水平触发。


    epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&ev);     // 把需要监视的listenfd和它的事件加入epollfd中。
    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);  interest list: 关注列表,  performed 履行, 表演,扮演
    // The event argument describes the object linked to the file descriptor fd. The events member is a bit mask composed by ORing together zero or more of the following available event types:
    struct epoll_event evs[10];      // 存放epoll_wait()返回事件的数组。

    while (true)        // 事件循环。
    {
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        int infds=epoll_wait(epollfd,evs,10,-1);       // 等待监视的fd有事件发生, 此时 受到监视的fd只有epollfd的 listenfd 监视的读事件
        // When successful, epoll_wait() returns the number of file descriptors ready for the requested I/O,
        // 返回失败。
        if (infds < 0)
        {
            perror("epoll_wait() failed"); break;
        }

        // 超时。
        if (infds == 0)
        {
            printf("epoll_wait() timeout.\n"); continue;
        }

        // 如果infds>0，表示有事件发生的fd的数量。
        for (int ii=0;ii<infds;ii++)       // 遍历epoll返回的数组evs。
        {
            if (evs[ii].data.fd==listenfd)   // 如果是listenfd有事件，表示有新的客户端连上来。
            {
                ////////////////////////////////////////////////////////////////////////
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);
                int clientfd = accept(listenfd,(struct sockaddr*)&clientaddr,&len);
                //accept()函数成功返回一个新的套接字的描述符, 该套接字将与客户端套接字进行数据传输
                setnonblocking(clientfd);         // 客户端连接的fd必须设置为非阻塞的。

                printf ("accept client(fd=%d,ip=%s,port=%d) ok.\n",clientfd,inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
                
                // 为新客户端连接准备读事件，并添加到epoll中。
                ev.data.fd=clientfd;
                ev.events=EPOLLIN|EPOLLET;           // 边缘触发。
                epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd,&ev);
                ////////////////////////////////////////////////////////////////////////
            }
            else // 如果是客户端连接的fd有事件。
            {
                ////////////////////////////////////////////////////////////////////////
                if (evs[ii].events & EPOLLRDHUP)                     // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
                {//EPOLLRHDHUP表示当与对端连接的套接字（socket）被远程关闭时，会触发 epoll_wait() 返回一个可读事件。这个事件通常用于检测连接的远程端是否关闭。
                    printf("1client(eventfd=%d) disconnected.\n",evs[ii].data.fd);
                    close(evs[ii].data.fd);            // 关闭客户端的fd。
                }                                //  普通数据  带外数据
                else if (evs[ii].events & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
                { //EPOLLIN 表示套接字可以进行非阻塞读取操作，而 EPOLLPRI 表示存在紧急数据等待读取。
                    char buffer[1024];
                    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
                    {    
                        bzero(&buffer, sizeof(buffer));
                        ssize_t nread = read(evs[ii].data.fd, buffer, sizeof(buffer));    // 这行代码用了read()，也可以用recv()，一样的，不要纠结。
                        if (nread > 0)      // 成功的读取到了数据。
                        {
                            // 把接收到的报文内容原封不动的发回去。
                            printf("recv(eventfd=%d):%s\n",evs[ii].data.fd,buffer);
                            send(evs[ii].data.fd,buffer,strlen(buffer),0);
                        } 
                        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。EINTR：表示中断错误。
                        {  
                            continue;
                        } 
                        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
                        {/*EAGAIN 或 EWOULDBLOCK：表示资源暂时不可用或者操作会导致阻塞。这个错误码通常在非阻塞套接字、文件描述符或者其他 I/O 操作中使用。*/
                            break;
                        } 
                        else if (nread == 0)  // 客户端连接已断开。
                        {  
                            printf("2client(eventfd=%d) disconnected.\n",evs[ii].data.fd);
                            close(evs[ii].data.fd);            // 关闭客户端的fd。
                            break;
                        }
                    }
                }
                else if (evs[ii].events & EPOLLOUT)                  // 有数据需要写，暂时没有代码，以后再说。
                {
                }
                else                                                                    // 其它事件，都视为错误。
                {
                    printf("3client(eventfd=%d) error.\n",evs[ii].data.fd);
                    close(evs[ii].data.fd);            // 关闭客户端的fd。
                }
                ////////////////////////////////////////////////////////////////////////
            }
        }
    }

  return 0;
}
