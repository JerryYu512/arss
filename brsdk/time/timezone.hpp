/**
 * MIT License
 * 
 * Copyright © 2021 <wotsen>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file timezone.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <time.h>
#include <memory>
#include "brsdk/mix/copyable.hpp"

namespace brsdk {

// TimeZone for 1970~2030
class TimeZone : public copyable {
public:
    /**
     * @brief 使用时区文件进行初始化，如：/usr/share/zoneinfo/Asia/Shanghai
     * 
     * @param zonefile 时区文件
     */
    explicit TimeZone(const char* zonefile);

    /**
     * @brief 使用时区具体时差，默认上海时区
     * 
     * @param eastOfUtc 相对0时时差，s
     * @param tzname 时区名称
     */
    TimeZone(int eastOfUtc = 8 * 3600, const char* tzname = "CST");
    TimeZone() = default;

    /**
     * @brief 是否有效
     * 
     * @return true 
     * @return false 
     */
    bool valid(void) const {
        return static_cast<bool>(data_);
    }

    /**
     * @brief 转换到本地时间
     * 
     * @param secondsSinceEpoch 相对0时的秒数
     * @return struct tm 本地时间
     */
    struct tm toLocalTime(time_t secondsSinceEpoch) const;

    /**
     * @brief 从本地时间转换到秒
     * 
     * @param tm 本地时间
     * @return time_t 秒
     */
    time_t fromLocalTime(const struct tm& tm) const;

    /**
     * @brief 转换到utc时间, @ref gmtime(3)
     * 
     * @param secondsSinceEpoch 相对0时的秒数
     * @param yday 是否填充一年的天数
     * @return struct tm 
     */
    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);

    /**
     * @brief utc时间转换到秒, @ref timegm(3)
     * 
     * @param tm utc时间
     * @return time_t 秒
     */
    static time_t fromUtcTime(const struct tm& tm);

    /**
     * @brief 由utc时间转到秒 year in [1900..2500], month in [1..12], day in [1..31]
     * 
     * @param year 年
     * @param month 月
     * @param day 日
     * @param hour 时
     * @param minute 分
     * @param seconds 秒
     * @return time_t 秒
     */
    static time_t fromUtcTime(int year, int month, int day, int hour, int minute, int seconds);

    /**
     * @brief 当前系统的时区差值
     * 
     * @return time_t 秒
     */
    static time_t timezoneOffset(void);

    /**
     * @brief 获取系统时区名称缩写
     * 
     * @return const char* 缩写，eg:CST
     */
    static const char* TimeZone::timezoneName(void);

    struct Data;

private:
    std::shared_ptr<Data> data_; ///< 数据
};

} // namespace brsdk
