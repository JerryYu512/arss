/**
 * MIT License
 *
 * Copyright © 2021 <Jerry.Yu>.
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
 * @file evloop.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <atomic>
#include <functional>
#include <vector>

#include "arss/mix/types.hpp"
#include "arss/lock/mutex.hpp"
#include "arss/thread/current_thread.hpp"
#include "arss/time/timestamp.hpp"
#include "ev_timerId.hpp"
#include "ev_type.hpp"

namespace arss {

namespace net {

class EvChannel;
class EvPoller;
class EvTimerQueue;

///
/// Reactor, at most one per thread.
///
/// This is an interface class, so don't expose too much details.
class EventLoop : noncopyable {
public:
    using EvFunctor = std::function<void()>;

    EventLoop();
    ~EventLoop();  // force out-line dtor, for std::unique_ptr members.

    ///
    /// Loops forever.
    ///
    /// Must be called in the same thread as creation of the object.
    ///
    void loop();

    /// Quits loop.
    ///
    /// This is not 100% thread safe, if you call through a raw pointer,
    /// better to call through shared_ptr<EventLoop> for 100% safety.
    void quit();

    ///
    /// Time when poll returns, usually means data arrival.
    ///
    Timestamp pollReturnTime() const { return pollReturnTime_; }

    int64_t iteration() const { return iteration_; }

    /// Runs callback immediately in the loop thread.
    /// It wakes up the loop, and run the cb.
    /// If in the same loop thread, cb is run within the function.
    /// Safe to call from other threads.
    void runInLoop(EvFunctor cb);
    /// Queues callback in the loop thread.
    /// Runs after finish pooling.
    /// Safe to call from other threads.
    void queueInLoop(EvFunctor cb);

    size_t queueSize() const;

    // timers

    ///
    /// Runs callback at 'time'.
    /// Safe to call from other threads.
    ///
    EvTimerId runAt(Timestamp time, EvTimerCallback cb);
    ///
    /// Runs callback after @c delay seconds.
    /// Safe to call from other threads.
    ///
    EvTimerId runAfter(double delay, EvTimerCallback cb);
    ///
    /// Runs callback every @c interval seconds.
    /// Safe to call from other threads.
    ///
    EvTimerId runEvery(double interval, EvTimerCallback cb);
    ///
    /// Cancels the timer.
    /// Safe to call from other threads.
    ///
    void cancel(EvTimerId timerId);

    // internal usage
    void wakeup();
    void updateChannel(EvChannel* channel);
    void removeChannel(EvChannel* channel);
    bool hasChannel(EvChannel* channel);

    // pid_t threadId() const { return threadId_; }
    void assertInLoopThread() {
        if (!isInLoopThread()) {
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const { return threadId_ == thread::tid(); }
    // bool callingPendingFunctors() const { return callingPendingFunctors_; }
    bool eventHandling() const { return eventHandling_; }

    void setContext(const Any& context) { context_ = context; }

    const Any& getContext() const { return context_; }

    Any* getMutableContext() { return &context_; }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();
    void handleRead();  // waked up
    void doPendingFunctors();

    void printActiveChannels() const;  // DEBUG

    typedef std::vector<EvChannel*> EvChannelList;

    bool looping_; /* atomic */
    std::atomic<bool> quit_;
    bool eventHandling_;          /* atomic */
    bool callingPendingFunctors_; /* atomic */
    int64_t iteration_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    std::unique_ptr<EvPoller> poller_;
    std::unique_ptr<EvTimerQueue> timerQueue_;
    int wakeupFd_;
    // unlike in TimerQueue, which is an internal class,
    // we don't expose EvChannel to client.
    std::unique_ptr<EvChannel> wakeupChannel_;
    Any context_;

    // scratch variables
    EvChannelList activeChannels_;
    EvChannel* currentActiveChannel_;

    mutable MutexLock mutex_;
    std::vector<EvFunctor> pendingFunctors_ GUARDED_BY(mutex_);
};

}  // namespace net

}  // namespace arss
