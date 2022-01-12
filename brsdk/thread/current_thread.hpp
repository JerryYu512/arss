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
 * @file current_thread.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#pragma once

#include "brsdk/mix/types.hpp"
#include <string>

namespace brsdk {

namespace thread {

// internal，线程变量

// 线程id
extern __thread int t_cachedTid;

// 线程id字符串
extern __thread char t_tidString[32];

// 线程id字符串长度
extern __thread int t_tidStringLength;

// 线程名称
extern __thread const char* t_threadName;

/**
 * @brief 获取线程id
 * 
 * @return int 线程id
 */
int tid(void);

///< tid字符串
static inline const char* tidString(void)
{
    return t_tidString;
}

///< tid长度
static inline int tidStringLength(void)
{
    return t_tidStringLength;
}

///< 线程名
static inline const char* name() { return t_threadName; }

/**
 * @brief 是否为主线程
 * 
 * @return true 是
 * @return false 否
 */
bool isMainThread(void);

/**
 * @brief 当前线程休眠
 * 
 * @param usec 时间值，us
 */
void sleepUsec(int64_t usec);

/**
 * @brief 当前线程栈信息
 * 
 * @param demangle 是否记录帧信息
 * @return std::string 栈信息字符串
 */
std::string stackTrace(bool demangle);

}  // namespace thread

}  // namespace brsdk
