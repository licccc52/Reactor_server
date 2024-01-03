#include "InetAddress.h"

/*
class InetAddress
{
private:
    sockaddr_in addr_;        // 表示地址协议的结构体。
public:
    InetAddress(const std::string &ip,uint16_t port);      // 如果是监听的fd，用这个构造函数。
    InetAddress(const sockaddr_in addr):addr_(addr){}  // 如果是客户端连上来的fd，用这个构造函数。
    ~InetAddress();

    const char *ip() const;                // 返回字符串表示的地址，例如：192.168.150.128
    uint16_t    port() const;              // 返回整数表示的端口，例如：80、8080
    const sockaddr *addr() const;   // 返回addr_成员的地址，转换成了sockaddr。
};
*/
InetAddress::InetAddress(const std::string &ip, uint16_t port){// 如果是监听的fd，用这个构造函数。
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const sockaddr_in addr):addr_(addr){// 如果是客户端连上来的fd，用这个构造函数。

}

InetAddress::~InetAddress(){
    
}

const char *InetAddress::ip() const{
    return inet_ntoa(addr_.sin_addr);
}


uint16_t InetAddress::port() const{
    return ntohs(addr_.sin_port);
}              

const sockaddr *InetAddress::addr() const{
    return (sockaddr*)&addr_;
}