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
#include "arss/mix/copyable.hpp"
#include "arss/mix/types.hpp"
#include <time.h>

namespace arss {

///
/// Date in Gregorian calendar.
///
/// This class is immutable.
/// It's recommended to pass it by value, since it's passed in register on x64.
///
class Date : public copyable {
public:
    struct YearMonthDay {
        int year;   // [1900..2500]
        int month;  // [1..12]
        int day;    // [1..31]
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    ///
    /// Constucts an invalid Date.
    ///
    Date() : julianDayNumber_(0) {}

    ///
    /// Constucts a yyyy-mm-dd Date.
    ///
    /// 1 <= month <= 12
    Date(int year, int month, int day);

    ///
    /// Constucts a Date from Julian Day Number.
    ///
    explicit Date(int julianDayNum) : julianDayNumber_(julianDayNum) {}

    ///
    /// Constucts a Date from struct tm
    ///
    explicit Date(const struct tm&);

    // default copy/assignment/dtor are Okay

    void swap(Date& that) { std::swap(julianDayNumber_, that.julianDayNumber_); }

    bool valid() const { return julianDayNumber_ > 0; }

    ///
    /// Converts to yyyy-mm-dd format.
    ///
    std::string toIsoString() const;

    struct YearMonthDay yearMonthDay() const;

    int year() const { return yearMonthDay().year; }

    int month() const { return yearMonthDay().month; }

    int day() const { return yearMonthDay().day; }

    // [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
    int weekDay() const { return (julianDayNumber_ + 1) % kDaysPerWeek; }

    int julianDayNumber() const { return julianDayNumber_; }

private:
    int julianDayNumber_;
};

inline bool operator<(Date x, Date y) { return x.julianDayNumber() < y.julianDayNumber(); }

inline bool operator==(Date x, Date y) { return x.julianDayNumber() == y.julianDayNumber(); }

} // namespace arss
