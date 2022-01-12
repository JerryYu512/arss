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
 * @file logging.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "brsdk/time/timestamp.hpp"
#include "brsdk/time/timezone.hpp"
#include "logstream.hpp"

///< 模块名称
#ifndef CUSTOM_MODULE_NAME
#define CUSTOM_MODULE_NAME "NON"
#endif

namespace brsdk {

// brsdk::Logger::setLogLevel(brsdk::Logger::TRACE);
// brsdk::Logger::setTimeZone(brsdk::TimeZone(8*3600, "CST"));

class Logger {
public:
    enum LogLevel {
        TRACE,
        LOG_LV_DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    // compile time calculation of basename of source file
    class SourceFile {
    public:
        template <int N>
        SourceFile(const char (&arr)[N]) : data_(arr), size_(N - 1) {
            const char* slash = strrchr(data_, '/');  // builtin function
            if (slash) {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char* filename) : data_(filename) {
            const char* slash = strrchr(filename, '/');
            if (slash) {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(const char* mname, SourceFile file, int line);
    Logger(const char* mname, SourceFile file, int line, LogLevel level);
    Logger(const char* mname, SourceFile file, int line, LogLevel level, const char* func);
    Logger(const char* mname, SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    ///< 日志等级
    static LogLevel logLevel();

    ///< 设置日志等级
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();

    ///< 设置日志输出自定义接口
    static void setOutput(OutputFunc);

    ///< 设置日志刷新接口
    static void setFlush(FlushFunc);

    ///< 设置时区
    static void setTimeZone(const TimeZone& tz);

    // TODO:Add module
private:
    class Impl {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const char* mname, const SourceFile& file, int line, const char* func);
        void formatTime();
        void finish();

        Timestamp time_;        ///< 时间戳
        LogStream stream_;      ///< 日志流
        LogLevel level_;        ///< 日志等级
        int line_;              ///< 行号
        SourceFile basename_;   ///< 文件名
        LogLevel logLevel_;
    };

    Impl impl_;
};

//
// CAUTION: do not write:
//
// if (good)
//   LOG_INFO << "Good news";
// else
//   LOG_WARN << "Bad news";
//
// this expends to
//
// if (good)
//   if (logging_INFO)
//     logInfoStream << "Good news";
//   else
//     logWarnStream << "Bad news";
//
#define LOG_TRACE                                          \
    if (brsdk::Logger::logLevel() <= brsdk::Logger::TRACE) \
    brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__, brsdk::Logger::TRACE, __func__).stream()
#define LOG_DEBUG                                          \
    if (brsdk::Logger::logLevel() <= brsdk::Logger::LOG_LV_DEBUG) \
    brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__, brsdk::Logger::LOG_LV_DEBUG, __func__).stream()
#define LOG_INFO \
    if (brsdk::Logger::logLevel() <= brsdk::Logger::INFO) brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__).stream()
#define LOG_WARN brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__, brsdk::Logger::WARN).stream()
#define LOG_ERROR brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__, brsdk::Logger::ERROR).stream()
#define LOG_FATAL brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__, brsdk::Logger::FATAL).stream()

#define LOG_SYSERR brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL brsdk::Logger(CUSTOM_MODULE_NAME, __FILE__, __LINE__, true).stream()

///< 内存分配错误
#define LOG_MEMALLOC_FAILED(size) LOG_ERROR << "alloc memory failed, size = " << size << ", reason : " << brsdk::strerror_tl(brsdk::get_errno()) << "\n";
///< 参数为空
#define LOG_PARAM_NULL(ptr) LOG_ERROR << #ptr << " = " << ptr << ", maybe null\n"
///< 超出范围
#define LOG_PARAM_OVER_RANGE(v, min, max) LOG_ERROR << #v << " not in range[" << min << "," << max << "]\n"

///< 系统错误码
int get_errno(void);

///< 转换为字符串
const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
    brsdk::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(brsdk::Logger::SourceFile file, int line, const char* names, T* ptr) {
    if (ptr == NULL) {
        brsdk::Logger(CUSTOM_MODULE_NAME, file, line, brsdk::Logger::FATAL).stream() << names;
    }
    return ptr;
}

} // namespace brsdk
