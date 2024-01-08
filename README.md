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


# TCP的粘包和分包
1. 粘包: tcp接收到数据之后, 有序放在接收缓冲区中, 数据之间不存在分隔符的说法, 如果接收方没有及时的从缓冲区中取走数据, 看上去就像粘在了一起.
2. 分包: tcp报文的大小缺省是1460字节, 如果发送缓冲区的数据超过1460字节, tcp将拆分成多个包发送, 如果接收方及时的从接收缓冲区中取走了数据, 看上去就像是接收到了多个报文
-> 解决方法 : 
1) 采用固定长度的报文.
2) 在报文前面加上报文长度. 报文头部(4字节的整数) + 报文内容
3) 报文之间用分隔符. http协议 \r\n\r\n

