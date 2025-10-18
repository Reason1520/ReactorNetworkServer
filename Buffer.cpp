#include "Buffer.h"

// 构造函数
Buffer::Buffer() {

}

// 析构函数
Buffer::~Buffer() {

}

// 把数据追加到Buffer中
void Buffer::append(const char *data, size_t size) {
    m_buffer.append(data, size);
}

// 把数据追加到Buffer中,并附加报文头部
void Buffer::appendWithHead(const char *data, size_t size) {
    m_buffer.append((char *)&size, 4);  // 添加报文头部
    m_buffer.append(data, size);        // 添加报文数据
}

// 删除Buffer中数据,从pos开始,长度为len
void Buffer::erase(size_t pos, size_t len) {
    m_buffer.erase(pos, len);
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