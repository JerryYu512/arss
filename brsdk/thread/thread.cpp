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
 * @file thread.cpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-24
 *
 * @copyright MIT License
 *
 */
#include "thread.hpp"
#include <type_traits>
#include "brsdk/err/exception.hpp"
#include "brsdk/log/logging.hpp"
#include "current_thread.hpp"
#include "thread_util.hpp"

#include <errno.h>
#include <linux/unistd.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

namespace brsdk {

namespace thread {

namespace detail {

pid_t gettid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }

void afterFork() {
    t_cachedTid = 0;
    t_threadName = "main";
    tid();
    // no need to call pthread_atfork(NULL, NULL, &afterFork);
}

class ThreadNameInitializer {
public:
    ThreadNameInitializer() {
        t_threadName = "main";
        tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init;

struct ThreadData {
    ThreadFunc func_;
    void* arg_;            ///< 参数
    std::string name_;
    pid_t* tid_;
    CountDownLatch* latch_;

    ThreadData(ThreadFunc func, const std::string& name,
               pid_t* tid, CountDownLatch* latch)
        : func_(std::move(func)), name_(name), tid_(tid), latch_(latch) {}

    void runInThread() {
        *tid_ = tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;

        t_threadName = name_.empty() ? "brsdkThread" : name_.c_str();
        ::prctl(PR_SET_NAME, t_threadName);
        try {
            func_();
            t_threadName = "finished";
        } catch (const Exception& ex) {
            t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
            abort();
        } catch (const std::exception& ex) {
            t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        } catch (...) {
            t_threadName = "crashed";
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw;  // rethrow
        }
    }
};

void* startThread(void* obj) {
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

}  // namespace detail

void cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%6d", t_cachedTid);
    }
}

bool isMainThread() { return tid() == ::getpid(); }

void sleepUsec(int64_t usec) {
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

std::atomic_int32_t Thread::numCreated_;

Thread::Thread(ThreadFunc func, const std::string& n)
    : started_(false),
      joined_(false),
      attr_(NULL),
      pthreadId_(0),
      tid_(0),
      func_(std::move(func)),
      name_(n),
      latch_(1) {
    setDefaultName();
}

Thread::Thread(ThreadFunc fun, pthread_attr_t *attr, const std::string& name)
    : started_(false),
      joined_(false),
      attr_(attr),
      pthreadId_(0),
      tid_(0),
      func_(std::move(fun)),
      name_(name),
      latch_(1)
{

    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_) {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName() {
    int num = numCreated_;
    if (name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    // FIXME: move(func_)
    detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthreadId_, attr_, &detail::startThread, data)) {
        started_ = false;
        delete data;  // or no delete?
        LOG_SYSFATAL << "Failed in pthread_create";
    } else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join(void **ret) {
    assert(started_);
    assert(!joined_);
    // 已经调用过了。
    if (joined_) {
        return -1;
    }
    joined_ = true;
    return pthread_join(pthreadId_, ret);
}

// detach
int Thread::detach(void) {
    assert(started_);
    assert(!joined_);
    // 调用了joined_
    if (joined_) {
        return -1;
    }
    pthread_attr_t attr;
    int det= -1;
    memset(&attr, 0, sizeof(attr));
    // 获取属性
    if (attribute(attr) < 0 || pthread_attr_getdetachstate(&attr, &det) < 0) {
        return -1;
    }

    // 判断是不是已经分离
    if (det == (int)PTHREAD_CREATE_DETACHED) {
        return -1;
    }
    
    joined_ = true;
    return pthread_detach(pthreadId_);
}

// 绑定cpu
int Thread::bind_cpu(int cpu) {
    if (pthreadId_ > 0) {
        thread_bind_cpu(pthreadId_, cpu);
    }
    return -1;
}

// 让出执行
int Thread::yield(void) {
    return thread_yield();
}

// 发信号
int Thread::signal(int signal) {
    if (pthreadId_ > 0) {
        return thread_send_signal(pthreadId_, signal);
    }
    return -1;
}

// 是否存活
bool Thread::alive(void) {
    if (pthreadId_ > 0) {
        return thread_alive(pthreadId_);
    }
    return false;
}

// 取消线程
int Thread::cancel(void) {
    if (pthreadId_ > 0) {
        return thread_cancel(pthreadId_);
    }
    return -1;
}

// 获取属性
int Thread::attribute(pthread_attr_t &attr) {
    return pthread_getattr_np(pthreadId_, &attr);
}

}  // namespace thread

}  // namespace brsdk
