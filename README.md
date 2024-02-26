# Reactor Web Server
## 此项目对一些网络函数进行了封装

### InetAddress: 地址协议类

封装了 sockaddr_in 结构体, 设置两个构造函数, 一个为了监听的fd, 另一个为了客户端连上来的fd


### Socket类: 负责管理各类文件描述符(fd) 

封装了socket(), bind() listen() accept()等网络函数


### Epoll类:封装了epoll的三个函数
    epoll_create() : Epoll::Epoll()
    epoll_ctl() : updatechannel(), removechannel()
    epoll_wait() : vector<Channel*> Epoll::loop() 返回事件发生的vector


封装了地址协议(Socket,Epoll), 隐藏了底层语言的实现细节, 精简主程序, 增加了可读性
---------------------------------------------------------------------------


### Channel类: fd的保镖, 也相当于fd的管家 一对一服务
```c++
struct epoll_event
{
    uint32_t events; //指定事件
    epoll_data_t data; //携带的数据
};

union epoll_data
{
    void *ptr;
    int fd;
    uint32_t u32; 
    uint64_t u64;
}

```
epoll_data中的void *ptr比fd携带的数据多, Channel类的作用类似于fd的保镖, 用回调函数的形式像Channel对象注册各类事件的处理函数


### EventLoop类:  封装了Epoll类对象和事件循环

### TcpServer类: 封装了EventLoop对象的类, 一个TcpServer可以有多个事件循环

### Acceptor类: 由服务器端监听的Channel封装而成


Channel类封装了监听fd和客户端连接的fd, 监听的fd与客户端连接的fd的功能是不同的, 监听的fd与客户端连接的fd的声明周期也不同

### Connection类: 封装服务器端用于通信的Channel ,由客户端连接的Channel封装而成,

Channel类是Connection类的底层类, Connection类是TcpServer类的底层类
建立Tcp连接和释放Tcp连接都需要两次回调函数

1. 封装网络地址

2. 封装Socket

3. 封装Epoll

4. 封装一个Channel类, 使epoll_event中的epoll_data中的ptr指向这个Channel类对象, 存储信息更多 

5. EventLoop: 把Epoll类的对象和事件循环封装成类

6. TcpServer类:

7. Acceptor类, 对监听Socket listenfd进行封装

8. Connection类, 对连接上来的clientfd进行封装

9. 优化回调函数的结构, 管理Connection类和Channel类和 TcpServer类, 在TcpServer类中添加map容器管理 连接中的connection, 不要的connection在回调函数中删除

10. Acceptor, Buffer, Channel, Connection, Epoll, EventLoop, InetAddress, Socket, TcpServer(最上层的类)都是底层类

# TCP的粘包和分包
1. 粘包: tcp接收到数据之后, 有序放在接收缓冲区中, 数据之间不存在分隔符的说法, 如果接收方没有及时的从缓冲区中取走数据, 看上去就像粘在了一起.
2. 分包: tcp报文的大小缺省是1460字节, 如果发送缓冲区的数据超过1460字节, tcp将拆分成多个包发送, 如果接收方及时的从接收缓冲区中取走了数据, 看上去就像是接收到了多个报文
-> 解决方法 : 
1) 采用固定长度的报文.
2) 在报文前面加上报文长度. 报文头部(4字节的整数) + 报文内容
3) 报文之间用分隔符. http协议 \r\n\r\n

# 为什么要增加工作线程
1. Acceptor运行在主Reactor(主进程)中, Connection运行在从Reactor(进程池)中
2. 一个从Reactor负责多个Connection, 每个Connection的工作内容包括IO和计算(处理客户端要求). IO不会阻塞事件循环, 但是, 计算可能会阻塞事件循环. 如果计算阻塞了事件循环, 那么在同一Reactor中的全部Connection将会被阻塞
 -> 解决方式 : 分配器Acceptor把Connection分配给从Reactor, 从Reactor运行在线程池中, 有很多个, 此时IO和计算都在从Reactor中, 此时可以把计算的过程分离出来, 把计算的工作交给工作线程(workthread), 让工作线程去处理业务, 从Reactor只负责IO, 以免从Reactor阻塞在计算上.

 # 为什么要清理空闲的Connection对象
1. 空闲的connection对象是指长事件没有进行通讯的tcp连接
2. 空闲的connection对象会占用资源, 需要定期清理
3. 避免攻击, 攻击者可以利用Connection对象不释放的特点进行大量tcp连接, 占用服务器端资源

# 多线程资源管理Connection对象 -> shared_ptr
 共享指针(shared_ptr), 共享指针会记录有多少个共享指针指向同一个物体, 当这个物体数量将为0的时候, 程序就会自动释放这个物体, 省去程序员手动delete的环节
 PS: 如果一块资源同时有裸指针和共享指针指向它的时候, 那么当所有的共享指针都被摧毁, 但是裸指针仍然存在的时候, 这个裸指针底下的子隐患仍然会被释放, 此时再用裸指针去访问那块资源就变成了未定义的行为,会导致很严重的后果.

# 项目结构
 基础类 
 Socket : fd_ ,  ip_ , port_ 
 Channel : fd_ , EventLoop*, 通过回调函数控制读写事件的执行, 处理读写epoll_wait()返回之后, 控制事件的执行
 TcpServer 的构造函数创建 Acceptor实例, setnewconnection (TcpServer::newconnection),   在newconnection中accept(), 创建connection, 设置conn实例的回调函数, 将新创建的conn实例加入到 TcpServer::map中,
 Acceptor 创建的时候 执行socket(), bind(), listne(); 

 Connection::onmessage() 读事件完成 -> TcpServer:: onmessage(), 准备回送的信息, 添加报文头部 -> Connection::send(), 添加数据到outputbuffer_, 注册写事件, 把注册后的event_添加到epoll的红黑树上
 Connection::writecallback()


使用outputbuffer_缓冲区发送数据, 
 server.Start() -> tcpserver_.start_run() -> mainloop_->run(); EventLoop::run() // 运行事件循环, std::vector<Channel*> channels = ep_->loop(), Epoll::loop()//把有时间发生的fd添加到vector<Channel*>; // 存放epoll_wait() 返回事件, // 遍历epoll返回的数组evs, ch->handleevent();遍历事件 -> Channel::handleevent() -> Connection::writecallback()"只有Connection有发送和读取缓冲区" -> ::send(), 然后清空缓冲区,注销写事件,clientchannel_->disabelwriting();


# 程序主体结构
 主事件循环负责客户的连接, 然后把客户端的连接分配给从事件循环, 从事件循环运行在线程池中, 称为IO线程, IO线程接收到客户端的请求报文之后, 把它交给工作线程去计算, 工作线程计算完之后, 把响应报文直接发送给客户端
 
 # 异步唤醒循环
 目标 : 在工作线程中把发送数据的操作通过队列交给事件循环, 然后唤醒事件循环, 让事件循环执行发送的任务

# mutex不可以拷贝, 可以将mutex改为引用传递参数
### std::bind( , , ), bind函数
第一个函数是成员函数的地址, 第二个参数是对象的地址(需要普通指针)
![回调过程](/home/lichuang/Reactor_server/recall_path.png)


# 代码现在的未解决的bug Ubuntu 2004
在EventLoop()函数中: 在map conns_已经为空的情况下, 执行else{} 中的for循环的时候还是会进入, 最终导致段错误
```cpp
    if(mainloop_){
        // printf("主事件循环的闹钟时间到了。\n");
    }
    else
    {
        printf("EventLoop::handletimer() thread is %ld. fd ",syscall(SYS_gettid));
        time_t now = time(0); //获取当前事件
        for(auto aa:conns_){
            if (aa.first == 0) {
            // 跳过键为 0 的键值对
                std::cout << "Int EventLoop::handletimer() conns_ map , aa.first is 0 " << ", conns_ is empty()? , conns_.empty() : " << conns_.empty() << std::endl; 
                //Connection对象已析构
                // Int EventLoop::handletimer() conns_ map , aa.first is 0 , conns_ is empty()? , conns_.empty() : 1
                // 段错误
                continue;
            }
            //遍历map容器, 显示容器中每个Connection的fd()
            std::cout << "EventLoop::handletimer()  conns_ : aa.first:  " <<  aa.first <<",  aa.second : " << aa.second << std::endl;
            if(aa.second->timeout(now, timeout_)){
                printf("EventLoop::handletimer()1 erase thread is %ld.\n",syscall(SYS_gettid)); 
                {
                    std::lock_guard<std::mutex> gd(mmutex_);
                    conns_.erase(aa.first); //从map容器中删除超时的conn
                }
                timercallback_(aa.first); //从TcpServer的map中删除超时的conn
            }
        }
        printf("\n");
    }
```

# 服务程序的退出
1. 设置2和15的信号
2. 在信号处理函数中停止主从时间循环和工作循环
3. 服务程序主动退出
