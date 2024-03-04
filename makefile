all:client tcpepoll

client:client.cpp
	g++ -g -o client client.cpp 
# tcpepoll:tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp ThreadPool.cpp Timestamp.cpp
# 	g++ -g -o tcpepoll tcpepoll.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp ThreadPool.cpp  Timestamp.cpp -lpthread

# clean:
# 	rm -f client tcpepoll
# 目标文件列表
OBJS = InetAddress.o Socket.o Epoll.o Channel.o EventLoop.o TcpServer.o Acceptor.o Connection.o Buffer.o EchoServer.o ThreadPool.o Timestamp.o

# 静态库文件名
LIBRARY = libtcpepoll.a

# 编译器选项
CXXFLAGS = -g

# 静态库的编译规则
$(LIBRARY): $(OBJS)
	ar rcs $@ $^

# client 可执行文件的编译规则
client: client.cpp
	g++ $(CXXFLAGS) -o client client.cpp

# tcpepoll 可执行文件的编译规则，链接静态库
tcpepoll: tcpepoll.cpp $(LIBRARY)
	g++ $(CXXFLAGS) -g -o tcpepoll tcpepoll.cpp -L. -ltcpepoll -lpthread

# 清理规则
clean:
	rm -f client tcpepoll $(LIBRARY) $(OBJS)
