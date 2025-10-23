#include "TimeStamp.h"

// 使用现在的时间初始化TimeStamp
TimeStamp::TimeStamp() {
    m_time_since_epoch = time(0);
}

// 使用指定的时间初始化TimeStamp
TimeStamp::TimeStamp(int64_t time): m_time_since_epoch(time) {

}

// 返回当前时间的TimeStamp
TimeStamp TimeStamp::now() {
    return TimeStamp();
}

// 返回int格式的时间
time_t TimeStamp::toint() const {
    return m_time_since_epoch;
}

// 返回字符串格式的时间
std::string TimeStamp::toString() const {
    char buf[64] = {0};
    tm *tm_time = localtime(&m_time_since_epoch);
    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900,
             tm_time->tm_mon + 1,
             tm_time->tm_mday,
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec);
    return buf;
}