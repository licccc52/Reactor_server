Reactor Web Server
#此项目对各个网络库进行了封装

1. 封装网络地址

2. 封装Socket

3. 封装Epoll

4. 封装一个Channel类, 使epoll_event中的epoll_data中的ptr指向这个Channel类对象, 存储信息更多 

5. EventLoop: 把Epoll类的对象和事件循环封装成类

6. TcpServer类:

7. Acceptor类, 对监听Socket listenfd进行封装

8. Connection类, 对连接上来的clientfd进行封装

9. 优化回调函数的结构, 管理Connection类和Channel类和 TcpServer类, 在TcpServer类中添加map容器管理 连接中的connection, 不要的connection在回调函数中删除