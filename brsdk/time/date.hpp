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
 * @file date.hpp
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
#include "brsdk/mix/copyable.hpp"
#include "brsdk/mix/types.hpp"
#include "timestamp.hpp"

namespace brsdk {

/**
 * @brief 格林威治日期，儒略日计数
 * 
 */
class Date : public copyable {
public:
    struct YearMonthDay {
        int year;   // [1900..2500]
        int month;  // [1..12]
        int day;    // [1..31]
    };

    static const int kDaysPerWeek = 7;          ///< 每星期的天数
    static const int kJulianDayOf1970_01_01;    ///< 起始儒略日，格林威治时间的中午开始

    ///
    /// 非法日期
    ///
    Date() : julianDayNumber_(0) {}

    ///
    /// 类似 yyyy-mm-dd
    ///
    /// 1 <= month <= 12
    Date(int year, int month, int day);

    ///
    /// 指定儒略日开始
    ///
    explicit Date(int julianDayNum) : julianDayNumber_(julianDayNum) {}

    ///
    /// 指定时间
    ///
    explicit Date(const struct tm&);

    /**
     * @brief 交换时间
     * 
     * @param that 
     */
    void swap(Date& that) { std::swap(julianDayNumber_, that.julianDayNumber_); }

    /**
     * @brief 判断是否合法时间
     * 
     * @return true 
     * @return false 
     */
    bool valid() const { return julianDayNumber_ > 0; }

    /**
     * @brief 转换到 yyyy-mm-dd 日期格式
     * 
     * @return std::string 日期
     */
    std::string toIsoString() const;

    /**
     * @brief 获取年月日结构体
     * 
     * @return struct YearMonthDay 日期值
     */
    struct YearMonthDay yearMonthDay() const;

    ///< 年
    int year() const { return yearMonthDay().year; }

    ///< 月
    int month() const { return yearMonthDay().month; }

    ///< 日
    int day() const { return yearMonthDay().day; }

    ///< [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
    int weekDay() const { return (julianDayNumber_ + 1) % kDaysPerWeek; }

    ///< 儒略日
    int julianDayNumber() const { return julianDayNumber_; }

    /**
     * @brief 一月的天数
     *
     * @param year 年份
     * @param month 月份
     * @return int 天数
     */
    static int days_of_month(int year, int month) {
        if (month < 1 || month > 12) {
            return 0;
        }

        static const unsigned char days[2][13] = {
            {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
            {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
        return days[month == 2 && BRSDK_IS_LEAP_YEAR(year)][month];
    }

    /**
     * @brief 英文月份字符串转数字
     *
     * @param month 月
     * @return int 月
     */
    static int month_atoi(const char* month);

    /**
     * @brief 月份数字转英文字符串
     *
     * @param month 月
     * @return const char* 月
     */
    static const char* month_itoa(int month);

    /**
     * @brief 英文星期字符串转数字
     *
     * @param weekday 周
     * @return int 周
     */
    static int weekday_atoi(const char* weekday);

    /**
     * @brief 星期数字转英文字符串
     *
     * @param weekday 周
     * @return int 周
     */
    static const char* weekday_itoa(int weekday);

private:
    int julianDayNumber_;   ///< 儒略日
};

inline bool operator<(Date x, Date y) { return x.julianDayNumber() < y.julianDayNumber(); }

inline bool operator==(Date x, Date y) { return x.julianDayNumber() == y.julianDayNumber(); }

}  // namespace brsdk
