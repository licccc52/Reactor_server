#include"Buffer.h"

//private:
    // std::string buf_;    //用于存放数据

Buffer::Buffer()
{

}

Buffer::~Buffer()
{
    
}

void Buffer::append(const char *data, size_t size)     //把数据追加到buf_中
{
    buf_.append(data, size);
}

void Buffer::appendwithhead(const char *data, size_t size)//把数据追加到buf_中, 附加报文头部
{
    buf_.append((char*)&size, 4);  //处理报文头
    buf_.append(data, size);   //添加报文本体
}

// 从buf_的pos开始，删除nn个字节，pos从0开始。
void Buffer::erase(size_t pos,size_t nn)                             
{
    buf_.erase(pos,nn);
}

size_t Buffer::size()
{
    return buf_.size();
}
                                  //返回buf_的大小
const char* Buffer::data()
{
    return buf_.data(); // 将内容以字符数组形式返回 字符数组 char[];
}
                             //返回buf_的首地址
void Buffer::clear()
{
    buf_.clear();
}  