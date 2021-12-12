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
 * @file date.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "date.hpp"
#include <string.h>
#include <assert.h>
#include <stdio.h>  // snprintf

namespace brsdk {

static const char* s_weekdays[] = {"Sunday",   "Monday", "Tuesday", "Wednesday",
                                   "Thursday", "Friday", "Saturday"};

static const char* s_months[] = {"January",   "February", "March",    "April",
                                 "May",       "June",     "July",     "August",
                                 "September", "October",  "November", "December"};

namespace detail {

char require_32_bit_integer_at_least[sizeof(int) >= sizeof(int32_t) ? 1 : -1];

// algorithm and explanation see:
// http://www.faqs.org/faqs/calendars/faq/part2/
// http://blog.csdn.net/Solstice

int getJulianDayNumber(int year, int month, int day) {
    (void)require_32_bit_integer_at_least;  // no warning please
    int a = (14 - month) / 12;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    return day + (153 * m + 2) / 5 + y * 365 + y / 4 - y / 100 + y / 400 - 32045;
}

struct Date::YearMonthDay getYearMonthDay(int julianDayNumber) {
    int a = julianDayNumber + 32044;
    int b = (4 * a + 3) / 146097;
    int c = a - ((b * 146097) / 4);
    int d = (4 * c + 3) / 1461;
    int e = c - ((1461 * d) / 4);
    int m = (5 * e + 2) / 153;
    Date::YearMonthDay ymd;
    ymd.day = e - ((153 * m + 2) / 5) + 1;
    ymd.month = m + 3 - 12 * (m / 10);
    ymd.year = b * 100 + d - 4800 + (m / 10);
    return ymd;
}
}  // namespace detail

using namespace detail;

const int Date::kJulianDayOf1970_01_01 = detail::getJulianDayNumber(1970, 1, 1);

Date::Date(int y, int m, int d) : julianDayNumber_(getJulianDayNumber(y, m, d)) {}

Date::Date(const struct tm& t)
    : julianDayNumber_(getJulianDayNumber(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday)) {}

std::string Date::toIsoString() const {
    char buf[32];
    YearMonthDay ymd(yearMonthDay());
    snprintf(buf, sizeof buf, "%4d-%02d-%02d", ymd.year, ymd.month, ymd.day);
    return buf;
}

Date::YearMonthDay Date::yearMonthDay() const { return getYearMonthDay(julianDayNumber_); }

int Date::month_atoi(const char* month) {
    for (size_t i = 0; i < 12; ++i) {
        if (strncasecmp(month, s_months[i], strlen(month)) == 0) return i + 1;
    }
    return 0;
}

const char* Date::month_itoa(int month) {
    assert(month >= 1 && month <= 12);
    return s_months[month - 1];
}

int Date::weekday_atoi(const char* weekday) {
    for (size_t i = 0; i < 7; ++i) {
        if (strncasecmp(weekday, s_weekdays[i], strlen(weekday)) == 0) return i;
    }
    return 0;
}

const char* Date::weekday_itoa(int weekday) {
    assert(weekday >= 0 && weekday <= 7);
    if (weekday == 7) weekday = 0;
    return s_weekdays[weekday];
}

} // namespace brsdk
