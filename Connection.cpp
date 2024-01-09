#include"Connection.h"
#include<iostream>

Connection::Connection(EventLoop *loop, Socket *clientsock):loop_(loop), clientsock_(clientsock)
{
    if(loop == nullptr || clientsock == nullptr){
        std::cout << "! ----- In Connection::Connection , loop or clientsock is null ---- ! " << std::endl;
    }
    // 为新客户端连接准备读事件，并添加到epoll中。
    clientchannel_ = new Channel(loop_, clientsock_->fd());
    clientchannel_->setreadcallback(std::bind(&Connection::onmessage,this));
    clientchannel_->setclosecallback(std::bind(&Connection::closecallback,this));
    clientchannel_->seterrorcallback(std::bind(&Connection::errorcallback,this));
    clientchannel_->setwritecallback(std::bind(&Connection::writecallback,this));

    clientchannel_->useet();                //客户端练上来的fd采用边缘触发
    clientchannel_->enablereading();        //让epoll_wait()监视clientchannel的读事件
    
}


Connection::~Connection()
{
    delete clientchannel_;
    delete clientsock_;
}


int Connection::fd() const              // 返回fd_成员。
{
    return clientsock_->fd();
}

std::string Connection::ip() const     //返回fd_成员
{
    return clientsock_->ip();
}

uint16_t Connection::port() const      //返回port_成员
{
    return clientsock_->port();
}


void Connection::closecallback()       //TCP连接关闭(断开)的回调函数, 供Channel回调
{
    // std::cout << "回调函数 Connection::closecallback() " << std::endl;
    // printf("client(eventfd=%d) error.\n", fd());
    // close(fd());
    closecallback_(this);  //回调std::bind(&TcpServer::closeconnection, this, std::placeholders::_1)
}
void Connection::errorcallback()       //TCP连接错误的回调函数, 共Channel回调
{
    // std::cout << "回调函数 Connection::errorcallback() " << std::endl;
    // printf("client(eventfd=%d) error\n", fd());
    // close(fd());
    errorcallback_(this);//回调std::bind(&TcpServer::errorconnection, this, std::placeholders::_1)
}

void Connection::writecallback()       //处理写事件的回调函数, 供Channel回调
{
    int written = ::send(fd(), outputbuffer_.data(), outputbuffer_.size(), 0); //尝试把outputbuffer_中的数据全部发送出去
    if(written > 0) outputbuffer_.erase(0, written);         //从outputbuffer_中删除已成功发送的字节数

    //如果发送缓冲区中没有数据了, 表示数据已发送成功
    if(outputbuffer_.size() == 0) clientchannel_->disabelwriting();
}


//设置关闭fd_的回调函数
void Connection::setclosecallback(std::function<void(Connection*)> fn)
{
    closecallback_ = fn;
}
void Connection::seterrorcallback(std::function<void(Connection*)> fn)
{
    errorcallback_ = fn;
}

void Connection::setonmessagecallback(std::function<void(Connection*, std::string)> fn)
{
    onmessagecallback_ = fn;
}


void Connection::onmessage() 
{
    ////////////////////////////////////////////////////////////////////////
    char buffer[1024];
    while (true)             // 由于使用非阻塞IO，一次读取buffer大小数据，直到全部的数据读取完毕。
    {    
        bzero(&buffer, sizeof(buffer));
        ssize_t nread = read(fd(), buffer, sizeof(buffer));
        if (nread > 0)      // 成功的读取到了数据。
        {
            // 把接收到的报文内容原封不动的发回去。
            // printf("recv(eventfd=%d):%s\n",fd(),buffer);
            // send(fd(),buffer,strlen(buffer),0);
            inputbuffer_.append(buffer, nread);  // 把读取的数据追加到接收缓冲区中
        } 
        else if (nread == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {  
            continue;
        } 
        else if (nread == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            // printf("recv(eventfd=%d):%s\n",fd(), inputbuffer_.data());
            while(true){
                //////////////////////////////////////////////////////////////
                // 可以把以下代码封装在Buffer类中，还可以支持固定长度、指定报文长度和分隔符等多种格式。
                int len;
                memcpy(&len,inputbuffer_.data(),4);     // 从inputbuffer中获取报文头部。
                // 如果inputbuffer中的数据量小于报文头部，说明inputbuffer中的报文内容不完整。
                if (inputbuffer_.size()<len+4) break;

                std::string message(inputbuffer_.data()+4,len);   // 从inputbuffer中获取一个报文。
                inputbuffer_.erase(0,len+4);                                 // 从inputbuffer中删除刚才已获取的报文。
                //////////////////////////////////////////////////////////////

                printf("message (eventfd=%d):%s\n",fd(),message.c_str());

                /*
                // 在这里，将经过若干步骤的运算。
                message="reply:"+message;
                
                len=message.size();                        // 计算回应报文的大小。
                std::string tmpbuf((char*)&len,4);  // 把报文头部填充到回应报文中。
                tmpbuf.append(message);             // 把报文内容填充到回应报文中。
                
                send(fd(),tmpbuf.data(),tmpbuf.size(),0);   // 把临时缓冲区中的数据直接send()出去。
                */

               //此处的回调函数定义在TcpServer中, onmessage
               //调用此函数处理客户端发送来的报文, 计算回送的报文
                onmessagecallback_(this,message);       // 回调TcpServer::onmessage()。
            }
            break;
        } 
        else if (nread == 0)  // 客户端连接已断开。
        {  
            closecallback();
            break;
        }
    }
}


void Connection::send(const char *data, size_t size) //发送数据
{
    outputbuffer_.append(data, size); //把需要发送的数据保存到Connection的发送缓冲区中
    
    //注册写事件
    clientchannel_->enablewriting(); //注册写事件

}