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
#include "timezone.hpp"

namespace brsdk {

/// 一小时的秒数
#define BRSDK_SECONDS_PER_HOUR 3600ull
/// 一天的秒数, 24 * 3600
#define BRSDK_SECONDS_PER_DAY 86400ull
/// 一周的秒数, 7 * 24 * 3600
#define BRSDK_SECONDS_PER_WEEK 604800ull

/// 闰年判断
#define BRSDK_IS_LEAP_YEAR(y) (((y)&3) == 0 && ((y) % 100 != 0 || (y) % 400 == 0))

/// 时间格式
#define BRSDK_TIME_FMT "%02d:%02d:%02d"
/// 时间格式长度
#define BRSDK_TIME_FMT_BUFLEN 12

/// 日期格式
#define BRSDK_DATETIME_FMT "%04d-%02d-%02d %02d:%02d:%02d"
/// 日期格式长度
#define BRSDK_DATETIME_FMT_BUFLEN 24

/// UTC时间格式化
#define BRSDK_UTCIME_FMT "%4d-%2d-%2dT%2d:%2d:%2dZ"
/// UTC时间格式长度
#define BRSDK_UTCIME_FMT_BUFLEN 32

/// 格林威治时间格式化
#define BRSDK_GMTIME_FMT "%.3s %.3s %02d %02d:%02d:%02d %.3s %04d"
/// 格林威治时间格式长度
#define BRSDK_GMTIME_FMT_BUFLEN 32

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

/**
 * @brief UTC时间戳，使用微秒实现
 * 
 */
class Timestamp : public copyable {
public:
    Timestamp() : microSecondsSinceEpoch_(0) {}
    Timestamp(const Timestamp& t) : microSecondsSinceEpoch_(t.microSecondsSinceEpoch_) {}
    Timestamp(Timestamp&& t) : microSecondsSinceEpoch_(t.microSecondsSinceEpoch_) {}
    Timestamp& operator=(const Timestamp& t) {
        microSecondsSinceEpoch_ = t.microSecondsSinceEpoch_;
        return *this;
    }

    /**
     * @brief 使用毫秒构造
     * 
     * @param microSecondsSinceEpochArg 毫秒
     */
    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        : microSecondsSinceEpoch_(microSecondsSinceEpochArg) {}

    void swap(Timestamp& that) { std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_); }

    /**
     * @brief 转为字符串：s.ms
     * 
     * @return std::string 
     */
    std::string toString() const;

    /**
     * @brief 转为格式化的UTC字符串 YYYY-MM-DD hh:mm:ss.xxxxxx
     * 
     * @param showMicroseconds 显示毫秒
     * @return std::string 
     */
    std::string toFormattedString(bool showMicroseconds = true) const;

    /**
     * @brief 转换为文件格式的字符串 YYYYMMDD_hhmmss_xxxxxx
     * 
     * @param showMicroseconds 显示毫秒
     * @return std::string 
     */
    std::string toFormattedFileString(bool showMicroseconds = true) const;

    /**
     * @brief 是否有效
     * 
     * @return true 
     * @return false 
     */
    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    /**
     * @brief 1970-01-01到现在的毫秒数
     * 
     * @return int64_t 
     */
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

    /**
     * @brief 1970-01-01到现在的秒数
     * 
     * @return int64_t 
     */
    time_t secondsSinceEpoch() const {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    /**
     * @brief 当前时间
     * 
     * @return Timestamp 
     */
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

    /**
     * @brief 由unix时间转换
     * 
     * @param t 秒
     * @return Timestamp 
     */
    static Timestamp fromUnixTime(time_t t) { return fromUnixTime(t, 0); }

    /**
     * @brief 由unix时间转换
     * 
     * @param t 秒
     * @param microseconds 毫秒
     * @return Timestamp 
     */
    static Timestamp fromUnixTime(time_t t, int microseconds) {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }

    /**
     * @brief 天数延时
     *
     * @param day 天
     */
    static void ddelay(time_t day) { usleep(day * 24 * 60 * 60 * 1000 * 1000ull); }

    /**
     * @brief 小时延时
     *
     * @param hour
     */
    static void hdelay(time_t hour) { usleep(hour * 60 * 60 * 1000 * 1000ull); }

    /**
     * @brief 分钟延时
     *
     * @param min 分钟
     */
    static void mdelay(time_t min) { usleep(min * 60 * 1000 * 1000ull); }

    /**
     * @brief 秒延时
     *
     * @param sec 秒
     */
    static void sdelay(time_t sec) { usleep(sec * 1000 * 1000ull); }

    /**
     * @brief 毫秒延时
     *
     * @param ms 毫秒
     */
    static void msdelay(time_t ms) { usleep(ms * 1000ull); }

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
        return tv.tv_sec * 1000ull + tv.tv_usec / 1000ull;
    }

    /**
     * @brief 获取当前时间戳
     *
     * @return unsigned int 时间戳
     */
    static unsigned long long gettick(void) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000ull + ts.tv_nsec / 1000000ull;
    }

    /**
     * @brief 获取当前时间纳秒
     *
     * @return unsigned long long 纳秒
     */
    static unsigned long long gethrtime_us(void) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return ts.tv_sec * 1000000ull + ts.tv_nsec / 1000ull;
    }

    /**
     * @brief 本地日期转时间秒
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
     * @brief 日期格式化, %04d-%02d-%02d %02d:%02d:%02d
     *
     * @param dt 日期
     * @param buf 输出buf
     * @return char* 结果
     */
    static char* datetime_fmt(datetime_t* dt, char* buf);

    /**
     * @brief 时间格式化 "%02d:%02d:%02d"
     *
     * @param sec 秒
     * @param buf 输出buf
     * @return char* 结果
     */
    static char* duration_fmt(time_t sec, char* buf);
    static char* duration_fmt(struct tm* tm, char* buf);

    /**
     * @brief 格林威治时间格式化, %.3s %.3s %02d %02d:%02d:%02d GMT %04d
     *
     * @param time 时间，秒
     * @param buf 输出buf
     * @return char* 结果
     */
    static char* gmtime_fmt(time_t time, char* buf, const char* zonename);
    static char* gmtime_fmt(time_t time, char* buf) {
        return gmtime_fmt(time, buf, TimeZone::timezoneName());
    }
    static char* gmtime_fmt(struct tm* time, char* buf, const char* zonename);
    static char* gmtime_fmt(struct tm* time, char* buf) {
        return gmtime_fmt(time, buf, TimeZone::timezoneName());
    }

    /**
     * @brief UTC(iso8601，秒精度为0)时间格式化 %4d-%2d-%2dT%2d:%2d:%2d
     * 
     * @param time 时间，秒
     * @param buf 输出buf
     * @param timezone 时区分钟差值，mintue
     * @return char* 结果
     */
    static char* utctime_fmt(time_t time, char* buf, int timezone);

    /**
     * @brief 当前系统时区UTC(iso8601，秒精度为0)时间格式化 %4d-%2d-%2dT%2d:%2d:%2d
     * 
     * @param time 时间，秒
     * @param buf 输出buf
     * @param timezone 时区分钟差值，mintue
     * @return char* 结果
     */
    static char* utctime_fmt(time_t time, char* buf) {
        return utctime_fmt(time, buf, TimeZone::timezoneOffset() * 60);
    }

    static char* utctime_fmt(struct tm* time, char* buf);

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
    static const int kMicroSecondsPerSecond = 1000 * 1000; ///< 微秒精度

private:
    int64_t microSecondsSinceEpoch_;    ///< 1970-01-01到指定时间的微秒数
};

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

/**
 * @brief 比较时间差值
 * 
 * @param high 大的时间
 * @param low 小的时间
 * @return double 差值
 */
inline double timeDifference(const Timestamp& high, const Timestamp& low) {
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

/**
 * @brief 时间加法
 * 
 * @param timestamp 时间戳
 * @param seconds 秒，可以有小数
 * @return Timestamp 结果
 */
inline Timestamp addTime(const Timestamp& timestamp, const double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}  // namespace brsdk
