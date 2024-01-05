Reactor Web Server
#此项目对各个网络库进行了封装

1.封装网络地址

2.封装Socket

3.封装Epoll

4.封装一个Channel类, 使epoll_event中的epoll_data中的ptr指向这个Channel类对象, 存储信息更多 

5.EventLoop: 把Epoll类的对象和事件循环封装成类