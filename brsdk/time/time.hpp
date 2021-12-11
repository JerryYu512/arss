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
 * @file time.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>

namespace brsdk {

/// 一小时的秒数
#define BRSDK_SECONDS_PER_HOUR    3600
/// 一天的秒数, 24 * 3600
#define BRSDK_SECONDS_PER_DAY     86400
/// 一周的秒数, 7 * 24 * 3600
#define BRSDK_SECONDS_PER_WEEK    604800

/// 闰年判断
// #define ARS_IS_LEAP_YEAR(year) (((year) % 4 == 0 && (year) % 100 != 0) || (year) % 100 == 0)
#define BRSDK_IS_LEAP_YEAR(y) (((y) & 3) == 0 && ((y) % 100 != 0 || (y) % 400 == 0))

/**
 * @brief 时间
 * 
 */
typedef struct datetime_s {
    int year;       ///< 年
    int month;      ///< 月
    int day;        ///< 日
    int hour;       ///< 时
    int min;        ///< 分
    int sec;        ///< 秒
    int ms;         ///< 毫秒
} datetime_t;

/**
 * @brief 时间值，精确到微秒
 * 
 */
typedef struct {
    time_t sec;     ///< 秒
    time_t usec;    ///< 微秒
} timeval_t;

/**
 * @brief 时间值，精确到纳秒
 * 
 */
typedef struct {
    time_t sec;     ///< 秒
    time_t nsec;    ///< 纳秒
} timespec_t;

/**
 * @brief 一月的天数
 * 
 * @param year 年份
 * @param month 月份
 * @return int 天数
 */
int days_of_month(int year, int month);

/**
 * @brief 天数延时
 * 
 * @param day 天
 */
static inline void ddelay(time_t day) {
    usleep(day * 24 * 60 * 60 * 1000 * 1000);
}

/**
 * @brief 小时延时
 * 
 * @param hour 
 */
static inline void hdelay(time_t hour) {
    usleep(hour * 60 * 60 * 1000 * 1000);
}

/**
 * @brief 分钟延时
 * 
 * @param min 分钟
 */
static inline void mdelay(time_t min) {
    usleep(min * 60 * 1000 * 1000);
}

/**
 * @brief 秒延时
 * 
 * @param sec 秒
 */
static inline void sdelay(time_t sec) {
    usleep(sec * 1000 * 1000);
}

/**
 * @brief 毫秒延时
 * 
 * @param ms 毫秒
 */
static inline void msdelay(time_t ms) {
    usleep(ms * 1000);
}

/**
 * @brief 微妙延时
 * 
 * @param us 微妙
 */
static inline void usdelay(time_t us) {
    usleep(us);
}

/**
 * @brief 获取时间值，精确到微秒
 * 
 * @return timeval_t 时间
 */
static inline timeval_t gettime_val(void) {
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    timeval_t ret {
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
static inline timespec_t gettime_spec(void) {
    struct timespec sp;
    ::clock_gettime(CLOCK_MONOTONIC, &sp);
    timespec_t ret {
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
static inline unsigned long long gettimeofday_ms() {
    struct timeval tv;
    ::gettimeofday(&tv, NULL);
    return tv.tv_sec * (unsigned long long)1000 + tv.tv_usec/1000;
}

/**
 * @brief 获取当前时间戳
 * 
 * @return unsigned int 时间戳
 */
static inline unsigned int gettick(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/**
 * @brief 获取当前时间纳秒
 * 
 * @return unsigned long long 纳秒
 */
static inline unsigned long long gethrtime_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec*(unsigned long long)1000000 + ts.tv_nsec / 1000;
}

/**
 * @brief 当前时间秒
 * 
 * @return time_t 秒
 */
static inline time_t now(void) {
    return ::time(nullptr);
}

/**
 * @brief 获取当前日期
 * 
 * @return datetime_t 日期
 */
datetime_t time_now(void);

/**
 * @brief 日期转时间秒
 * 
 * @param dt 日期
 * @return time_t 秒
 */
time_t time_mktime(const datetime_t *dt);

/**
 * @brief 过去多少天的日期
 * 
 * @param dt 日期
 * @param days 过去的天数
 * @return datetime_t* 过去的日期
 */
datetime_t* datetime_past(datetime_t* dt, int days = 1);

/**
 * @brief 将来多少天的日期
 * 
 * @param dt 日期
 * @param days 未来的天数
 * @return datetime_t* 未来的日志
 */
datetime_t* datetime_future(datetime_t* dt, int days = 1);

/// 时间格式
#define BRSDK_TIME_FMT            "%02d:%02d:%02d"
/// 时间格式长度
#define BRSDK_TIME_FMT_BUFLEN     12

/**
 * @brief 时间格式化 "%02d:%02d:%02d"
 * 
 * @param sec 秒
 * @param buf 输出buf
 * @return char* 结果
 */
char* duration_fmt(time_t sec, char* buf);

/// 日期格式
#define BRSDK_DATETIME_FMT        "%04d-%02d-%02d %02d:%02d:%02d"
/// 日期格式长度
#define BRSDK_DATETIME_FMT_BUFLEN 24

/**
 * @brief 日期格式化, %04d-%02d-%02d %02d:%02d:%02d
 * 
 * @param dt 日期
 * @param buf 输出buf
 * @return char* 结果
 */
char* datetime_fmt(datetime_t* dt, char* buf);

/// 格林威治时间格式化
#define BRSDK_GMTIME_FMT          "%.3s, %02d %.3s %04d %02d:%02d:%02d GMT"
/// 格林威治时间格式长度
#define BRSDK_GMTIME_FMT_BUFLEN   30

/**
 * @brief 格林威治时间格式化, %.3s, %02d %.3s %04d %02d:%02d:%02d GMT
 * 
 * @param time 时间，秒
 * @param buf 输出buf
 * @return char* 结果
 */
char* gmtime_fmt(time_t time, char* buf);

/**
 * @brief 月份字符串转数字
 * 
 * @param month 月
 * @return int 月
 */
int month_atoi(const char* month);

/**
 * @brief 月份数字转字符串
 * 
 * @param month 月
 * @return const char* 月
 */
const char* month_itoa(int month);

/**
 * @brief 星期字符串转数字
 * 
 * @param weekday 周
 * @return int 周
 */
int weekday_atoi(const char* weekday);

/**
 * @brief 星期数字转字符串
 * 
 * @param weekday 周
 * @return int 周
 */
const char* weekday_itoa(int weekday);

/**
 * @brief 编译日期
 * 
 * @return datetime_t 日期
 */
datetime_t compile_datetime(void);

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
time_t cron_next_timeout(int minute, int hour, int day, int week, int month);

} // namespace brsdk
