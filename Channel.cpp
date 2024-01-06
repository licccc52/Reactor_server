#include"Channel.h"
#include<iostream>
/*
class Channel{
private:
    int fd_ = -1;//Channel拥有的fd, Channel和fd是一对一的关系
    Epoll *ep_ = nullptr; //Channel对应的红黑树, Channel和Epoll是多对一的关系, 一个Channel只对应一个Epoll
    bool inepoll = false; //Channel是否已经添加到epoll上, 如果未添加, 调用epoll_ctl()的时候用EPOLL_CTL_ADD, 否则使用EPOLL_CTL_MOD
    uint32_t events_ = 0;   // fd_需要监视的事件, listenfd和clientfd需要监视的EPOLLIN, clientfd还可能需要监视EPOLLOUT
    uint32_t revents_ = 0; // fd_已经发生的事件.


public:
    Channel(Epoll* ep, int fd); // 构造函数
    ~Channel(); // 析构函数

    int fd();   //返回fd_成员
    void useet(); // 采用边缘触发
    void enablereading(); // 让epoll_wait()监视fd_的读事件
    void setinepoll(); // 把inepoll_成员的值设置为true
    void setrevents(uint32_t ev); //设置revents_成员的值 为参数ev
    bool inepoll(); //返回inepoll_成员
    uint32_t events(); // 返回events_成员
    uint32_t revents(); // 返回revents_成员
};
*/

Channel::Channel(EventLoop* loop,int fd):loop_(loop),fd_(fd)      // 构造函数。
{
    std::cout << __FILE__ << " , "<< __LINE__ << ",   Channel Constructor" << std::endl;
}

Channel::~Channel()    //析构函数
{
    //在析构函数中, 不能销毁ep_, 不能关闭fd_, 因为这两个东西不属于channel类, Channel类知识需要他们, 需要使用这两个东西
}


int Channel::fd()   //返回fd_成员
{
    return fd_;
}


void Channel::useet() // 采用边缘触发
{
    events_ = events_|EPOLLET;
}

void Channel::enablereading() // 让epoll_wait()监视fd_的读事件
{
    events_ |= EPOLLIN;
    if(loop_ == nullptr){
        std::cout << "!! In CHANNEL::enablereading() ,Channel -> loop_ is nullptr  !! " << std::endl;
    }
    else loop_->updatechannel(this);
}

void Channel::setinepoll() // 把inepoll_成员的值设置为true
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev) //设置revents_成员的值 为参数ev
{
    revents_ = ev;
}

bool Channel::inepoll() //返回inepoll_成员
{
    return inepoll_;
}

uint32_t Channel::events() // 返回events_成员
{
    return events_;
}

uint32_t Channel::revents() // 返回revents_成员
{
    return revents_;
}

 void Channel::handleevent() //事件处理函数, epoll_wait() 返回的之后执行它
 { 
    if (revents_ & EPOLLRDHUP)    // 对方已关闭，有些系统检测不到，可以使用EPOLLIN，recv()返回0。
        {
            printf("1client(eventfd=%d) disconnected.\n",fd_);
            close(fd_);            // 关闭客户端的fd。
        }                                //  普通数据  带外数据
        else if (revents_ & (EPOLLIN|EPOLLPRI))   // 接收缓冲区中有数据可以读。
        {
            readcallback_();
        }
    else if (revents_ & EPOLLOUT)                  // 有数据需要写，暂时没有代码，以后再说。
    {
    }
    else                                                                   // 其它事件，都视为错误。
    {
        printf("3client(eventfd=%d) error.\n",fd_);
        close(fd_);            // 关闭客户端的fd。
    }
 }


// 处理新客户端连接请求
void Channel::newconnection(Socket* servsock) 
{
    ////////////////////////////////////////////////////////////////////////                    
    InetAddress clientaddr;// 注意，clientsock只能new出来，不能在栈上，否则析构函数会关闭fd。
    Socket *clientsock = new Socket(servsock->accept(clientaddr));
    printf ("Channel::newconnection :  accept client InetAddress Instance created (fd=%d,ip=%s,port=%d) ok.\n", clientsock->fd(),clientaddr.ip(),clientaddr.port());

    /*
    // 为新客户端连接准备读事件，并添加到epoll中。
    Channel* clientchannel = new Channel(loop_, clientsock->fd());
    clientchannel->setreadcallback(std::bind(&Channel::onmessage,clientchannel));
    clientchannel->useet();
    clientchannel->enablereading();
    //客户端连接上来的fd采用边缘触发
    ////////////////////////////////////////////////////////////////////////
    */

   Connection *conn = new Connection(loop_, clientsock);

}


// 处理对端发送过来的信息
void Channel::onmessage() 
{
    ////////////////////////////////////////////////////////////////////////
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd_, buffer, sizeof(buffer));
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            printf("recv(eventfd=%d):%s\n",fd_,buffer);
            send(fd_,buffer,strlen(buffer),0);
        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            printf("2client(eventfd=%d) disconnected.\n",fd_);
            close(fd_);            // 关闭客户端的fd。
            break;
        }
    }
}

 // 设置fd_读事件的回调函数。
 void Channel::setreadcallback(std::function<void()> fn)    
 {
    readcallback_=fn;
 }