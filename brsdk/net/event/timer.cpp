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
 * @file timer.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-30
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_loop.hpp"
#include "timer.hpp"
#include "event_channel.hpp"
#include <sys/timerfd.h>
#include <unistd.h>

namespace brsdk {

namespace net {

namespace {

// 创建时间句柄
int create_timerfd(void) {
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (timerfd < 0) {
		LOG_SYSFATAL << "Failed in timerfd_create";
	}

	return timerfd;
}

struct timespec how_much_time_from_now(Timestamp when) {
	int64_t ms = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();

	if (ms < 100) {
		ms = 100;
	}

	struct timespec ts;
	
	ts.tv_sec = static_cast<time_t>(ms / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<time_t>((ms % Timestamp::kMicroSecondsPerSecond) * 1000);

	return ts;
}

void read_timerfd(int timerfd, Timestamp now) {
	uint64_t howmany;
	ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));

	LOG_TRACE << "TimerRead " << howmany << " at " << now.toString();
	if (n != sizeof(howmany)) {
		LOG_ERROR << "TimerRead reads " << n << " bytes instead of 8";
	}
}

void reset_timerfd(int timerfd, Timestamp expiration) {
	struct itimerspec new_value;
	struct itimerspec old_value;

	memset(&new_value, 0, sizeof(new_value));
	memset(&old_value, 0, sizeof(old_value));

	new_value.it_value = how_much_time_from_now(expiration);
	int ret = ::timerfd_settime(timerfd, 0, &new_value, &old_value);
	if (ret) {
		LOG_SYSERR << "timerfd_settime()";
	}
}

} // namespace

void Timer::restart(Timestamp now) {
	if (repeat_) {
		expiration_ = addTime(now, interval_);
	} else {
		expiration_ = Timestamp::invalid();
	}
}

TimerQueue::TimerQueue(EventLoop* loop)
	: loop_(loop),
	  timerfd_(create_timerfd()),
	  timerfd_channel_(loop, timerfd_),
	  timers_(),
	  calling_expired_timers_(false) {
	timerfd_channel_.SetReadCallback(std::bind(&TimerQueue::HandleRead, this));
	timerfd_channel_.EnableReading();
}

TimerQueue::~TimerQueue() {
	timerfd_channel_.DisableAll();
	timerfd_channel_.remove();
	::close(timerfd_);
	for (const TimeEntry& timer : timers_) {
		delete timer.second;
	}
}

// 添加定时器
TimerId TimerQueue::AddTimer(TimerCallback cb, Timestamp when, double interval) {
	Timer* timer = new Timer(std::move(cb), when, interval);
	loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
	return TimerId(timer, timer->sequence_);
}

// 取消定时器
void TimerQueue::cancel(TimerId timer_id) {
	loop_->RunInLoop(std::bind(&TimerQueue::CancelInLoop, this, timer_id));
}

void TimerQueue::AddTimerInLoop(Timer* timer) {
	loop_->AssertInLoopThread();
	bool earliest_changed = insert(timer);

	if (earliest_changed) {
		reset_timerfd(timerfd_, timer->expiration_);
	}
}

void TimerQueue::CancelInLoop(TimerId timer_id) {
	loop_->AssertInLoopThread();
	assert(timers_.size() == activeTimers_.size());
	ActiveTimer timer(timer_id.timer_, timer_id.sequence_);
	auto it = activeTimers_.find(timer);

	if (it != activeTimers_.end()) {
		size_t n = timers_.erase(TimeEntry(it->first->sequence_, it->first));
		assert(n == 1);
		(void)n;
		delete it->first;
		activeTimers_.erase(it);
	} else if (calling_expired_timers_) {
		canceling_timers_.insert(timer);
	}

	assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::HandleRead(void) {
	loop_->AssertInLoopThread();
	Timestamp now(Timestamp::now());

	read_timerfd(timerfd_, now);

	std::vector<TimeEntry> expired = GetExpired(now);

	calling_expired_timers_ = true;
	canceling_timers_.clear();
	for (const TimeEntry& it : expired) {
		it.second->run();
	}
	calling_expired_timers_ = false;

	reset(expired, now);
}

std::vector<TimerQueue::TimeEntry> TimerQueue::GetExpired(Timestamp now) {
	assert(timers_.size() == activeTimers_.size());
	std::vector<TimerQueue::TimeEntry> expired;
	TimerQueue::TimeEntry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	auto end = timers_.lower_bound(sentry);
	std::copy(timers_.begin(), end, back_inserter(expired));
	timers_.erase(timers_.begin(), end);

	for (const TimerQueue::TimeEntry& it : expired) {
		ActiveTimer timer(it.second, it.second->sequence_);
		size_t n = activeTimers_.erase(timer);
		(void)n;
	}

	return expired;
}

void TimerQueue::reset(const std::vector<TimeEntry>& expired, Timestamp now) {
	Timestamp next_expire;

	for (const TimeEntry& it : expired) {
		ActiveTimer timer(it.second, it.second->sequence_);
		if (it.second->repeat_
			&& canceling_timers_.find(timer) == canceling_timers_.end()) {
			it.second->restart(now);
			insert(it.second);
		} else {
			delete it.second;
		}
	}

	if (!timers_.empty()) {
		next_expire = timers_.begin()->second->expiration_;
	}

	if (next_expire.valid()) {
		reset_timerfd(timerfd_, next_expire);
	}
}

bool TimerQueue::insert(Timer* timer) {
	loop_->AssertInLoopThread();
	bool earliest_changed = false;
	Timestamp when = timer->expiration_;
	auto it = timers_.begin();
	if (it == timers_.end() || when < it->first) {
		earliest_changed = true;
	}

	timers_.insert(TimeEntry(when, timer));

	activeTimers_.insert(ActiveTimer(timer, timer->sequence_));

	return earliest_changed;
}

} // namespace net

} // namespace brsdk
