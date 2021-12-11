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
 * @file async_logging.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-25
 *
 * @copyright MIT License
 *
 */
#pragma once
#include "brsdk/lock/countdownlatch.hpp"
#include "brsdk/lock/mutex.hpp"
#include "brsdk/thread/thread.hpp"
#include "logstream.hpp"

#include <atomic>
#include <vector>
#include <memory>

namespace brsdk {

class AsyncLogging : noncopyable {
public:
    AsyncLogging(const std::string& basename, off_t rollSize, int flushInterval = 3);

    ~AsyncLogging() {
        if (running_) {
            stop();
        }
    }

    void append(const char* logline, int len);

    void start() {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop() NO_THREAD_SAFETY_ANALYSIS {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    void threadFunc();

    typedef brsdk::detail::FixedBuffer<brsdk::detail::kLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    const int flushInterval_;
    std::atomic<bool> running_;
    const std::string basename_;
    const off_t rollSize_;
    thread::Thread thread_;
    CountDownLatch latch_;
    MutexLock mutex_;
    Condition cond_ GUARDED_BY(mutex_);
    BufferPtr currentBuffer_ GUARDED_BY(mutex_);
    BufferPtr nextBuffer_ GUARDED_BY(mutex_);
    BufferVector buffers_ GUARDED_BY(mutex_);
};

}  // namespace brsdk
