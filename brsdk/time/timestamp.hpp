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
 * @file timestamp.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "brsdk/mix/copyable.hpp"
#include "brsdk/mix/types.hpp"

namespace brsdk {

/// 一小时的秒数
#define BRSDK_SECONDS_PER_HOUR 3600
/// 一天的秒数, 24 * 3600
#define BRSDK_SECONDS_PER_DAY 86400
/// 一周的秒数, 7 * 24 * 3600
#define BRSDK_SECONDS_PER_WEEK 604800

/// 闰年判断
// #define ARS_IS_LEAP_YEAR(year) (((year) % 4 == 0 && (year) % 100 != 0) || (year) % 100 == 0)
#define BRSDK_IS_LEAP_YEAR(y) (((y)&3) == 0 && ((y) % 100 != 0 || (y) % 400 == 0))

/// 时间格式
#define BRSDK_TIME_FMT "%02d:%02d:%02d"
/// 时间格式长度
#define BRSDK_TIME_FMT_BUFLEN 12

/// 日期格式
#define BRSDK_DATETIME_FMT "%04d-%02d-%02d %02d:%02d:%02d"
/// 日期格式长度
#define BRSDK_DATETIME_FMT_BUFLEN 24

/// 格林威治时间格式化
#define BRSDK_GMTIME_FMT "%.3s, %02d %.3s %04d %02d:%02d:%02d GMT"
/// 格林威治时间格式长度
#define BRSDK_GMTIME_FMT_BUFLEN 30

/**
 * @brief 时间
 *
 */
typedef struct datetime_s {
    int year;   ///< 年
    int month;  ///< 月
    int day;    ///< 日
    int hour;   ///< 时
    int min;    ///< 分
    int sec;    ///< 秒
    int ms;     ///< 毫秒
} datetime_t;

/**
 * @brief 时间值，精确到微秒
 *
 */
typedef struct {
    time_t sec;   ///< 秒
    time_t usec;  ///< 微秒
} timeval_t;

/**
 * @brief 时间值，精确到纳秒
 *
 */
typedef struct {
    time_t sec;   ///< 秒
    time_t nsec;  ///< 纳秒
} timespec_t;

///
/// Time stamp in UTC, in microseconds resolution.
///
/// This class is immutable.
/// It's recommended to pass it by value, since it's passed in register on x64.
///
class Timestamp : public copyable {
public:
    ///
    /// Constucts an invalid Timestamp.
    ///
    Timestamp() : microSecondsSinceEpoch_(0) {}
    Timestamp(const Timestamp& t) : microSecondsSinceEpoch_(t.microSecondsSinceEpoch_) {}
    Timestamp(Timestamp&& t) : microSecondsSinceEpoch_(t.microSecondsSinceEpoch_) {}
    Timestamp& operator=(const Timestamp& t) {
        microSecondsSinceEpoch_ = t.microSecondsSinceEpoch_;
        return *this;
    }

    ///
    /// Constucts a Timestamp at specific time
    ///
    /// @param microSecondsSinceEpoch
    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        : microSecondsSinceEpoch_(microSecondsSinceEpochArg) {}

    void swap(Timestamp& that) { std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_); }

    // default copy/assignment/dtor are Okay

    std::string toString() const;
    std::string toFormattedString(bool showMicroseconds = true) const;
    std::string toFormattedFileString(bool showMicroseconds = true) const;

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    // for internal usage.
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    ///
    /// Get time of now.
    ///
    static Timestamp now();
    /**
     * @brief 当前时间秒
     *
     * @return time_t 秒
     */
    static time_t time(void) { return ::time(nullptr); }

    /**
     * @brief 获取当前日期
     *
     * @return datetime_t 日期
     */
    static datetime_t time_now(void);

    static Timestamp invalid() { return Timestamp(); }

    static Timestamp fromUnixTime(time_t t) { return fromUnixTime(t, 0); }

    static Timestamp fromUnixTime(time_t t, int microseconds) {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }

    /**
     * @brief 天数延时
     *
     * @param day 天
     */
    static void ddelay(time_t day) { usleep(day * 24 * 60 * 60 * 1000 * 1000); }

    /**
     * @brief 小时延时
     *
     * @param hour
     */
    static void hdelay(time_t hour) { usleep(hour * 60 * 60 * 1000 * 1000); }

    /**
     * @brief 分钟延时
     *
     * @param min 分钟
     */
    static void mdelay(time_t min) { usleep(min * 60 * 1000 * 1000); }

    /**
     * @brief 秒延时
     *
     * @param sec 秒
     */
    static void sdelay(time_t sec) { usleep(sec * 1000 * 1000); }

    /**
     * @brief 毫秒延时
     *
     * @param ms 毫秒
     */
    static void msdelay(time_t ms) { usleep(ms * 1000); }

    /**
     * @brief 微妙延时
     *
     * @param us 微妙
     */
    static void usdelay(time_t us) { usleep(us); }

    /**
     * @brief 获取时间值，精确到微秒
     *
     * @return timeval_t 时间
     */
    static timeval_t gettime_val(void) {
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        timeval_t ret{
            tv.tv_sec,
            tv.tv_usec,
        };
        return ret;
    }

    /**
     * @brief 获取时间值，精确到纳秒
     *
     * @return timespec_t 时间
     */
    static timespec_t gettime_spec(void) {
        struct timespec sp;
        ::clock_gettime(CLOCK_MONOTONIC, &sp);
        timespec_t ret{
            sp.tv_sec,
            sp.tv_nsec,
        };

        return ret;
    }

    /**
     * @brief 获取当前时间毫秒
     *
     * @return unsigned long long 毫秒
     */
    static unsigned long long gettimeofday_ms() {
        struct timeval tv;
        ::gettimeofday(&tv, NULL);
        return tv.tv_sec * (unsigned long long)1000 + tv.tv_usec / 1000;
    }

    /**
     * @brief 获取当前时间戳
     *
     * @return unsigned int 时间戳
     */
    static unsigned int gettick(void) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }

    /**
     * @brief 获取当前时间纳秒
     *
     * @return unsigned long long 纳秒
     */
    static unsigned long long gethrtime_us(void) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * (unsigned long long)1000000 + ts.tv_nsec / 1000;
    }

    /**
     * @brief 日期转时间秒
     *
     * @param dt 日期
     * @return time_t 秒
     */
    static time_t time_mktime(const datetime_t* dt);

    /**
     * @brief 过去多少天的日期
     *
     * @param dt 日期
     * @param days 过去的天数
     * @return datetime_t* 过去的日期
     */
    static datetime_t* datetime_past(datetime_t* dt, int days = 1);

    /**
     * @brief 将来多少天的日期
     *
     * @param dt 日期
     * @param days 未来的天数
     * @return datetime_t* 未来的日期
     */
    static datetime_t* datetime_future(datetime_t* dt, int days = 1);
    /**
     * @brief 时间格式化 "%02d:%02d:%02d"
     *
     * @param sec 秒
     * @param buf 输出buf
     * @return char* 结果
     */
    static char* duration_fmt(time_t sec, char* buf);
    /**
     * @brief 日期格式化, %04d-%02d-%02d %02d:%02d:%02d
     *
     * @param dt 日期
     * @param buf 输出buf
     * @return char* 结果
     */
    static char* datetime_fmt(datetime_t* dt, char* buf);
    /**
     * @brief 格林威治时间格式化, %.3s, %02d %.3s %04d %02d:%02d:%02d GMT
     *
     * @param time 时间，秒
     * @param buf 输出buf
     * @return char* 结果
     */
    static char* gmtime_fmt(time_t time, char* buf);
    /**
     * @brief 编译日期
     *
     * @return datetime_t 日期
     */
    static datetime_t compile_datetime(void);

    /*
     * minute   hour    day     week    month       action
     * 0~59     0~23    1~31    0~6     1~12
     *  30      -1      -1      -1      -1          cron.hourly
     *  30      1       -1      -1      -1          cron.daily
     *  30      1       15      -1      -1          cron.monthly
     *  30      1       -1       0      -1          cron.weekly
     *  30      1        1      -1      10          cron.yearly
     */
    /// 下次超时时间
    static time_t cron_next_timeout(int minute, int hour, int day, int week, int month);

public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microsecond
/// resolution for next 100 years.
inline double timeDifference(Timestamp high, Timestamp low) {
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
inline Timestamp addTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}  // namespace brsdk
