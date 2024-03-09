# Reactor Web Server 2
对Reactor Web Server(https://github.com/licccc52/Reactor_server/tree/main)进行了一些修改用以适配Mprpc项目

1. connection的超时机制

2. Buffer的append()函数和默认sep_(置为0), 不用分隔符显示缓冲区中的数据长度

3. 生成静态库放入rpc_project(https://github.com/licccc52/Rpc_project)中

4. 修改TcpConnection的构造函数和回调函数 -> 为了适配rpcprovider.cc中定义的部分函数

