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
    explicit TimeZone(const char* zonefile);
    TimeZone(int eastOfUtc, const char* tzname);  // a fixed timezone
    TimeZone() = default;                         // an invalid timezone

    // default copy ctor/assignment/dtor are Okay.

    bool valid() const {
        // 'explicit operator bool() const' in C++11
        return static_cast<bool>(data_);
    }

    struct tm toLocalTime(time_t secondsSinceEpoch) const;
    time_t fromLocalTime(const struct tm&) const;

    // gmtime(3)
    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
    // timegm(3)
    static time_t fromUtcTime(const struct tm&);
    // year in [1900..2500], month in [1..12], day in [1..31]
    static time_t fromUtcTime(int year, int month, int day, int hour, int minute, int seconds);

    struct Data;

private:
    std::shared_ptr<Data> data_;
};

} // namespace brsdk
