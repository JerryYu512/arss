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
 * @file timeiso8601.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace brsdk {

typedef struct {
    int64_t sec;    /* Number of seconds since the epoch of 1970-01-01T00:00:00Z */
    int32_t nsec;   /* Nanoseconds [0, 999999999] */
    int16_t offset; /* Offset from UTC in minutes [-1439, 1439] */
} timeiso8601_t;

/**
 * @brief iso8601
 * 
 */
class TimeISO8601 {
private:
    timeiso8601_t iso8601_; ///< 时间戳
    char time_[64];         ///< 时间戳，字符串

public:
    TimeISO8601(const std::string& str) : TimeISO8601(str.c_str(), str.length()) {}
    TimeISO8601(const char* str) : TimeISO8601(str, strlen(str)) {}
    TimeISO8601(const char* str, size_t len);
    TimeISO8601(const timeiso8601_t& tm);

    /**
     * @brief 获取时间戳
     * 
     * @return timeiso8601_t 
     */
    timeiso8601_t time(void);

    /**
     * @brief 时间格式化
     * 
     * @return std::string 
     */
    std::string format(void);

    /**
     * @brief 时间格式化
     * 
     * @param precision 小数部分精度(0-9)
     * @return std::string 
     */
    std::string format(int precision);

    /**
     * @brief 时间格式化
     * 
     * @param buf 内存
     * @param len 内存长度
     * @return true 成功
     * @return false 失败
     */
    bool format(char *buf, size_t len);

    /**
     * @brief 时间格式化
     * 
     * @param buf 内存
     * @param len 内存长度
     * @param precision 小数部分精度(0-9)
     * @return true 成功
     * @return false 失败
     */
    bool format(char *buf, size_t len, int precision);

    /**
     * @brief 转为utc时间
     * 
     * @return struct tm 
     */
    struct tm to_utc(void);

    /**
     * @brief 转为本地时间
     * 
     * @return struct tm 
     */
    struct tm to_local(void);

    /**
     * @brief 是否合法时间
     * 
     * @return true 合法
     * @return false 不合法
     */
    bool valid(void);

    /**
     * @brief 时间比较
     * 
     * @param tsp1 时间1
     * @param tsp2 时间2
     * @return int 0-相等，1-tsp1大，-1-tsp2大
     */
    static int compare(const timeiso8601_t& tsp1, const timeiso8601_t& tsp2) {
        if (tsp1.sec < tsp2.sec) return -1;
        if (tsp1.sec > tsp2.sec) return 1;
        if (tsp1.nsec < tsp2.nsec) return -1;
        if (tsp1.nsec > tsp2.nsec) return 1;
        return 0;
    }
};

inline bool operator==(const timeiso8601_t& tsp1, const timeiso8601_t& tsp2) {
    return 0 == TimeISO8601::compare(tsp1, tsp2);
}

inline bool operator<(const timeiso8601_t& tsp1, const timeiso8601_t& tsp2) {
    return -1 == TimeISO8601::compare(tsp1, tsp2);
}

inline bool operator>(const timeiso8601_t& tsp1, const timeiso8601_t& tsp2) {
    return 1 == TimeISO8601::compare(tsp1, tsp2);
}

} // namespace brsdk
