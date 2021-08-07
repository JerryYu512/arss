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

namespace arss {

typedef struct {
    int64_t sec;    /* Number of seconds since the epoch of 1970-01-01T00:00:00Z */
    int32_t nsec;   /* Nanoseconds [0, 999999999] */
    int16_t offset; /* Offset from UTC in minutes [-1439, 1439] */
} timeiso8601_t;

class TimeISO8601 {
public:
    /**
     * @brief 时间字符串解析
     * 
     * @param str ISO 8601字符串
     * @param len 字符串长度
     * @param tsp[out] 时间戳
     * @return true 成功
     * @return false 失败
     */
    static bool parse(const char* str, size_t len, timeiso8601_t* tsp);

    /**
     * @brief 时间格式化
     * 
     * @param dst[out] 存放字符串
     * @param len dst内存长度
     * @param tsp 时间戳
     * @return size_t 字符串长度
     */
    static size_t format(char* dst, size_t len, const timeiso8601_t* tsp);

    /**
     * @brief 时间格式化，更精确
     * 
     * @param dst[out] 存放字符串
     * @param len dst内存长度
     * @param tsp 时间戳
     * @param precision 纳秒及长度精度
     * @return size_t 字符串长度
     */
    static size_t format_precision(char* dst, size_t len, const timeiso8601_t* tsp, int precision);

    /**
     * @brief 时间比较
     * 
     * @param tsp1 时间1
     * @param tsp2 时间2
     * @return int 0-相等，1-tsp1大，-1-tsp2大
     */
    static int compare(const timeiso8601_t* tsp1, const timeiso8601_t* tsp2);

    /**
     * @brief 判断合法性
     * 
     * @param tsp 时间
     * @return true 合法
     * @return false 非法
     */
    static bool valid(const timeiso8601_t* tsp);

    /**
     * @brief 转为utc时间
     * 
     * @param tsp 时间戳
     * @param tmp tm时间
     * @return struct tm* 为空为失败
     */
    static struct tm* to_tm_utc(const timeiso8601_t* tsp, struct tm* tmp);

    /**
     * @brief 转为本地时间
     * 
     * @param tsp 时间戳
     * @param tmp tm时间
     * @return struct tm* 为空为失败
     */
    static struct tm* to_tm_local(const timeiso8601_t* tsp, struct tm* tmp);
};

} // namespace arss
