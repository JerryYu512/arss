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
 * @file timestamp.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "timestamp.hpp"
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <assert.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>
#include "date.hpp"

namespace brsdk {

#define EPOCH INT64_C(62135683200)      /* 1970-01-01T00:00:00 */
#define RDN_OFFSET INT64_C(62135683200) /* 1970-01-01T00:00:00 */
#define MIN_SEC INT64_C(-62135596800)   /* 0001-01-01T00:00:00 */
#define MAX_SEC INT64_C(253402300799)   /* 9999-12-31T23:59:59 */

static const char* s_weekdays[] = {"Sunday",   "Monday", "Tuesday", "Wednesday",
                                   "Thursday", "Friday", "Saturday"};

static const char* s_months[] = {"January",   "February", "March",    "April",
                                 "May",       "June",     "July",     "August",
                                 "September", "October",  "November", "December"};

static_assert(sizeof(Timestamp) == sizeof(int64_t), "Timestamp should be same size as int64_t");

std::string Timestamp::toString() const {
    char buf[32] = {0};
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string Timestamp::toFormattedString(bool showMicroseconds) const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if (showMicroseconds) {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d", tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
                 tm_time.tm_sec, microseconds);
    } else {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d", tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
                 tm_time.tm_sec);
    }
    return buf;
}

std::string Timestamp::toFormattedFileString(bool showMicroseconds) const {
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if (showMicroseconds) {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d_%02d%02d%02d_%06d", tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
                 tm_time.tm_sec, microseconds);
    } else {
        snprintf(buf, sizeof(buf), "%4d%02d%02d_%02d%02d%02d", tm_time.tm_year + 1900,
                 tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min,
                 tm_time.tm_sec);
    }
    return buf;
}

Timestamp Timestamp::now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

datetime_t Timestamp::time_now(void) {
    datetime_t dt;
    struct timeval tv;
    struct tm tm;

    gettimeofday(&tv, NULL);
    time_t tt = tv.tv_sec;
    localtime_r(&tt, &tm);

    dt.year = tm.tm_year + 1900;
    dt.month = tm.tm_mon + 1;
    dt.day = tm.tm_mday;
    dt.hour = tm.tm_hour;
    dt.min = tm.tm_min;
    dt.sec = tm.tm_sec;
    dt.ms = tv.tv_usec / 1000;

    return dt;
}

time_t Timestamp::time_mktime(const datetime_t* dt) {
    struct tm tm;
    time_t ts;
    ::time(&ts);
    struct tm* ptm = localtime(&ts);
    memcpy(&tm, ptm, sizeof(struct tm));
    tm.tm_yday = dt->year - 1900;
    tm.tm_mon = dt->month - 1;
    tm.tm_mday = dt->day;
    tm.tm_hour = dt->hour;
    tm.tm_min = dt->min;
    tm.tm_sec = dt->sec;
    return mktime(&tm);
}

datetime_t* Timestamp::datetime_future(datetime_t* dt, int days) {
    assert(days >= 0);
    int sub = days;
    int mdays;
    while (sub) {
        mdays = Date::days_of_month(dt->year, dt->month);
        if (dt->day + sub <= mdays) {
            dt->day += sub;
            break;
        }
        sub -= (mdays - dt->day + 1);
        if (++dt->month > 12) {
            dt->month = 1;
            ++dt->year;
        }
        dt->day = 1;
    }
    return dt;
}

datetime_t* Timestamp::datetime_past(datetime_t* dt, int days) {
    assert(days >= 0);
    int sub = days;
    while (sub) {
        if (dt->day > sub) {
            dt->day -= sub;
            break;
        }
        sub -= dt->day;
        if (--dt->month == 0) {
            dt->month = 12;
            --dt->year;
        }
        dt->day = Date::days_of_month(dt->year, dt->month);
    }
    return dt;
}

char* Timestamp::duration_fmt(time_t sec, char* buf) {
    size_t h, m, s;
    m = sec / 60;
    s = sec % 60;
    h = m / 60;
    m = m % 60;
    sprintf(buf, BRSDK_TIME_FMT, (int)h, (int)m, (int)s);
    return buf;
}

char* Timestamp::datetime_fmt(datetime_t* dt, char* buf) {
    sprintf(buf, BRSDK_DATETIME_FMT, dt->year, dt->month, dt->day, dt->hour, dt->min, dt->sec);
    return buf;
}

char* Timestamp::gmtime_fmt(time_t time, char* buf) {
    struct tm* tm = gmtime(&time);
    // strftime(buf, ARS_GMTIME_FMT_BUFLEN, "%a, %d %b %Y %H:%M:%S GMT", tm);
    sprintf(buf, BRSDK_GMTIME_FMT, s_weekdays[tm->tm_wday], tm->tm_mday, s_months[tm->tm_mon],
            tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buf;
}

datetime_t Timestamp::compile_datetime() {
    datetime_t dt;
    char month[32];
    sscanf(__DATE__, "%s %d %d", month, &dt.day, &dt.year);
    sscanf(__TIME__, "%d:%d:%d", &dt.hour, &dt.min, &dt.sec);
    dt.month = Date::month_atoi(month);
    return dt;
}

time_t Timestamp::cron_next_timeout(int minute, int hour, int day, int week, int month) {
    enum {
        UNKOWN,
        HOURLY,
        DAILY,
        WEEKLY,
        MONTHLY,
        YEARLY,
    } period_type = UNKOWN;
    struct tm tm;
    time_t tt;
    ::time(&tt);
    localtime_r(&tt, &tm);
    time_t tt_round = 0;

    tm.tm_sec = 0;
    if (minute >= 0) {
        period_type = HOURLY;
        tm.tm_min = minute;
    }
    if (hour >= 0) {
        period_type = DAILY;
        tm.tm_hour = hour;
    }
    if (week >= 0) {
        period_type = WEEKLY;
    } else if (day > 0) {
        period_type = MONTHLY;
        tm.tm_mday = day;
        if (month > 0) {
            period_type = YEARLY;
            tm.tm_mon = month - 1;
        }
    }

    if (period_type == UNKOWN) {
        return -1;
    }

    tt_round = mktime(&tm);
    if (week >= 0) {
        tt_round = tt + (week - tm.tm_wday) * BRSDK_SECONDS_PER_DAY;
    }
    if (tt_round > tt) {
        return tt_round;
    }

    switch (period_type) {
        case HOURLY:
            tt_round += BRSDK_SECONDS_PER_HOUR;
            return tt_round;
        case DAILY:
            tt_round += BRSDK_SECONDS_PER_DAY;
            return tt_round;
        case WEEKLY:
            tt_round += BRSDK_SECONDS_PER_WEEK;
            return tt_round;
        case MONTHLY:
            if (++tm.tm_mon == 12) {
                tm.tm_mon = 0;
                ++tm.tm_year;
            }
            break;
        case YEARLY:
            ++tm.tm_year;
            break;
        default:
            return -1;
    }

    return mktime(&tm);
}

} // namespace brsdk
