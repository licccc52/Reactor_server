# Reactor Web Server
## 此项目对各个网络库进行了封装

1. 封装网络地址

2. 封装Socket

3. 封装Epoll

4. 封装一个Channel类, 使epoll_event中的epoll_data中的ptr指向这个Channel类对象, 存储信息更多 

5. EventLoop: 把Epoll类的对象和事件循环封装成类

6. TcpServer类:

7. Acceptor类, 对监听Socket listenfd进行封装

8. Connection类, 对连接上来的clientfd进行封装

9. 优化回调函数的结构, 管理Connection类和Channel类和 TcpServer类, 在TcpServer类中添加map容器管理 连接中的connection, 不要的connection在回调函数中删除

10. Acceptor, Buffer, Channel, Connection, Epoll, EventLoop, InetAddress, Socket, TcpServer(最上层的类)都是底层类, 不涉及业务


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

# 项目结构
基础类 
Socket : fd_ ,  ip_ , port_ 
Channel : fd_ , EventLoop*, 通过回调函数控制读写事件的执行, 处理读写epoll_wait()返回之后, 控制事件的执行
TcpServer 的构造函数创建 Acceptor实例, setnewconnection (TcpServer::newconnection),   在newconnection中accept(), 创建connection, 设置conn实例的回调函数, 将新创建的conn实例加入到 TcpServer::map中,
Acceptor 创建的时候 执行socket(), bind(), listne(); 

Connection::onmessage() 读事件完成 -> TcpServer:: onmessage(), 准备回送的信息, 添加报文头部 -> Connection::send(), 添加数据到outputbuffer_, 注册写事件, 把注册后的event_添加到epoll的红黑树上
Connection::writecallback()


使用outputbuffer_缓冲区发送数据, 
EventLoop::run() // 运行事件循环, std::vector<Channel*> channels = ep_->loop(), Epoll::loop()//把有时间发生的fd添加到vector<Channel*>; // 存放epoll_wait() 返回事件, // 遍历epoll返回的数组evs, ch->handleevent();遍历事件 -> Channel::handleevent() -> Connection::writecallback()"只有Connection有发送和读取缓冲区" -> ::send(), 然后清空缓冲区,注销写事件,clientchannel_->disabelwriting();

# mutex不可以拷贝, 可以将mutex改为引用传递参数
![回调过程](/home/lichuang/Reactor_server/recall_path.png)
