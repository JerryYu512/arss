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
 * @file logging.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "logging.hpp"
#include "arss/thread/current_thread.hpp"
#include "arss/str/fmt.hpp"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

namespace arss {

/*
class LoggerImpl
{
 public:
  typedef Logger::LogLevel LogLevel;
  LoggerImpl(LogLevel level, int old_errno, const char* file, int line);
  void finish();

  Timestamp time_;
  LogStream stream_;
  LogLevel level_;
  int line_;
  const char* fullname_;
  const char* basename_;
};
*/

__thread char t_errnobuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
    "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL",
};

const char* strerror_tl(int savedErrno) {
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

Logger::LogLevel initLogLevel() {
    if (::getenv("MUDUO_LOG_TRACE"))
        return Logger::TRACE;
    else if (::getenv("MUDUO_LOG_DEBUG"))
        return Logger::LOG_LV_DEBUG;
    else
        return Logger::INFO;
}

// helper class for known string length at compile time
class T {
public:
    T(const char* str, unsigned len) : str_(str), len_(len) { assert(strlen(str) == len_); }

    const char* str_;
    const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v) {
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v) {
    s.append(v.data_, v.size_);
    return s;
}

void defaultOutput(const char* msg, int len) {
    size_t n = fwrite(msg, 1, len, stdout);
    // FIXME check n
    (void)n;
}

void defaultFlush() { fflush(stdout); }

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;
TimeZone g_logTimeZone;
Logger::LogLevel g_logLevel = initLogLevel();

Logger::Impl::Impl(LogLevel level, int savedErrno, const char* mname, const SourceFile& file, int line, const char *func)
    : time_(Timestamp::now()), stream_(), level_(level), line_(line), basename_(file) {
    formatTime();
    stream_ << "[" << T(LogLevelName[level], strlen(LogLevelName[level])) << "]";
    thread::tid();
    if (thread::t_threadName) {
        stream_ << "[" << thread::t_threadName << "]";
    }
    stream_ << "[" << mname << "]";
    stream_ << "[" << basename_;
    if (func) {
        stream_ << ":" << func;
    }
    stream_ << ":" << line_ << "] ";
    if (savedErrno != 0) {
        stream_ << strerror_tl(savedErrno) << "(errno=" << savedErrno << ")";
    }
}

void Logger::Impl::formatTime() {
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds =
        static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);
    if (seconds != t_lastSecond) {
        t_lastSecond = seconds;
        struct tm tm_time;
        if (g_logTimeZone.valid()) {
            tm_time = g_logTimeZone.toLocalTime(seconds);
        } else {
            ::gmtime_r(&seconds, &tm_time);  // FIXME TimeZone::fromUtcTime
        }

        int len = snprintf(t_time, sizeof(t_time), "%4d-%02d-%02d %02d:%02d:%02d",
                           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                           tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        (void)len;
    }

    if (g_logTimeZone.valid()) {
        str::Fmt us(".%03d ", microseconds);
        stream_ << T(t_time, strlen(t_time)) << T(us.data(), strlen(us.data()));
    } else {
        str::Fmt us(".%03dZ ", microseconds);
        stream_ << T(t_time, strlen(t_time)) << T(us.data(), strlen(us.data()));
    }
}

void Logger::Impl::finish() { }

Logger::Logger(const char* mname, SourceFile file, int line) : impl_(INFO, 0, mname, file, line, nullptr) {}

Logger::Logger(const char* mname, SourceFile file, int line, LogLevel level, const char* func)
    : impl_(level, 0, mname, file, line, func) {
}

Logger::Logger(const char* mname, SourceFile file, int line, LogLevel level) : impl_(level, 0, mname, file, line, nullptr) {}

Logger::Logger(const char* mname, SourceFile file, int line, bool toAbort)
    : impl_(toAbort ? FATAL : ERROR, errno, mname, file, line, nullptr) {}

Logger::~Logger() {
    impl_.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if (g_logLevel == FATAL) {
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level) { g_logLevel = level; }

Logger::LogLevel Logger::logLevel() { return g_logLevel; }

void Logger::setOutput(OutputFunc out) { g_output = out; }

void Logger::setFlush(FlushFunc flush) { g_flush = flush; }

void Logger::setTimeZone(const TimeZone& tz) { g_logTimeZone = tz; }

} // namespace arss
