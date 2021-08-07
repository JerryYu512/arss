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
 * @file mutex.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "arss/mix/noncopyable.hpp"
#include "arss/thread/current_thread.hpp"
#include "arss/defs/defs.hpp"
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

namespace arss {

// Use as data member of a class, eg.
//
// class Foo
// {
//  public:
//   int size() const;
//
//  private:
//   mutable MutexLock mutex_;
//   std::vector<int> data_ GUARDED_BY(mutex_);
// };
class CAPABILITY("mutex") MutexLock : noncopyable {
public:
    MutexLock() : holder_(0) { MCHECK(pthread_mutex_init(&mutex_, NULL)); }

    ~MutexLock() {
        assert(holder_ == 0);
        MCHECK(pthread_mutex_destroy(&mutex_));
    }

    // must be called when locked, i.e. for assertion
    bool isLockedByThisThread() const { return holder_ == thread::tid(); }

    void assertLocked() const ASSERT_CAPABILITY(this) { assert(isLockedByThisThread()); }

    // internal usage

    void lock() ACQUIRE() {
        MCHECK(pthread_mutex_lock(&mutex_));
        assignHolder();
    }

    void unlock() RELEASE() {
        unassignHolder();
        MCHECK(pthread_mutex_unlock(&mutex_));
    }

    pthread_mutex_t *getPthreadMutex() /* non-const */
    {
        return &mutex_;
    }

private:
    friend class Condition;

    class UnassignGuard : noncopyable {
    public:
        explicit UnassignGuard(MutexLock &owner) : owner_(owner) { owner_.unassignHolder(); }

        ~UnassignGuard() { owner_.assignHolder(); }

    private:
        MutexLock &owner_;
    };

    void unassignHolder() { holder_ = 0; }

    void assignHolder() { holder_ = thread::tid(); }

    pthread_mutex_t mutex_;
    pid_t holder_;
};

// Use as a stack variable, eg.
// int Foo::size() const
// {
//   MutexLockGuard lock(mutex_);
//   return data_.size();
// }
class SCOPED_CAPABILITY MutexLockGuard : noncopyable {
public:
    explicit MutexLockGuard(MutexLock &mutex) ACQUIRE(mutex) : mutex_(mutex) { mutex_.lock(); }

    ~MutexLockGuard() RELEASE() { mutex_.unlock(); }

private:
    MutexLock &mutex_;
};

// Prevent misuse like:
// MutexLockGuard(mutex_);
// A tempory object doesn't hold the lock for long!
#define MutexLockGuard(x) error "Missing guard object name"

} // namespace arss
