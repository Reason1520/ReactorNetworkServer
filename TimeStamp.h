#pragma once
#include <string>
#include <iostream>

class TimeStamp {
private:
    time_t m_time_since_epoch;

public:
    TimeStamp();                    // 使用现在的时间初始化TimeStamp
    TimeStamp(int64_t time);        // 使用指定的时间初始化TimeStamp

    static TimeStamp now();         // 返回当前时间的TimeStamp
    time_t toint() const;           // 返回int格式的时间
    std::string toString() const;   // 返回字符串格式的时间
};