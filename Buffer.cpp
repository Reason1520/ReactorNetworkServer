#include "Buffer.h"

// 构造函数
Buffer::Buffer(u_int16_t sep): m_sep(sep) {

}

// 析构函数
Buffer::~Buffer() {

}

// 把数据追加到Buffer中
void Buffer::append(const char *data, size_t size) {
    m_buffer.append(data, size);
}

// 把数据追加到Buffer中,并附加报文头部
void Buffer::appendWithSep(const char *data, size_t size) {
    if (m_sep == 0) {       // 没有报文头部
        m_buffer.append(data, size); // 添加报文数据
    }
    else if(m_sep == 1) {   // 四字节报文头部
        m_buffer.append((char *)&size, 4); // 添加报文头部
        m_buffer.append(data, size);
    }
    else if(m_sep == 2) {   // “\r\n\r\n”分隔符(http协议)
        // 有待完善
    }

}

// 删除Buffer中数据,从pos开始,长度为len
void Buffer::erase(size_t pos, size_t len) {
    m_buffer.erase(pos, len);
}

// 从Buffer中提取一条消息
bool Buffer::pickMessage(std::string &message) {
    if (m_buffer.size() == 0)
        return false;
    if (m_sep == 0)
    {
        message = m_buffer;
        m_buffer.clear();
    }
    else if (m_sep == 1) {
        int len;
        memcpy(&len, m_buffer.data(), 4);   // 从input_buffer中读取报文头部
        if (m_buffer.size() < len + 4) return false;

        message = m_buffer.substr(4, len);  // 从input_buffer中获取一个报文内容
        m_buffer.erase(0, len + 4);         // 从input_buffer中删除已处理的数据
    }
    else if (m_sep == 2) {
        // 有待完善
    }
    return true;
}

// 获取Buffer中数据的大小
size_t Buffer::size() {
    return m_buffer.size();
}

// 获取Buffer中数据的起始地址
const char *Buffer::data() {
    return m_buffer.data();
}

// 清空Buffer
void Buffer::clear() {
    m_buffer.clear();
}