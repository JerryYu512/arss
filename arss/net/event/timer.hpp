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
 * @file timer.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-07
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <atomic>
#include <map>
#include <set>
#include <vector>
#include "arss/time/timestamp.hpp"
#include "arss/mix/noncopyable.hpp"
#include "event_typedef.hpp"
#include "event_channel.hpp"

namespace arss {

namespace net {

class Timer : noncopyable {
public:
	Timer(TimerCallback cb, Timestamp when, double interval)
		: callback_(cb),
		  expiration_(when),
		  interval_(interval),
		  repeat_(interval > 0.0),
		  sequence_(++s_num_created_) {}
	
	friend class TimerQueue;

	void run(void) const {
		callback_();
	}

	Timestamp expiation(void) const {
		return expiration_;
	}

	bool repeat(void) const {
		return repeat_;
	}

	int64_t sequence(void) const {
		return sequence_;
	}

	void restart(Timestamp now);

	static int64_t NumCreated(void) {
		return s_num_created_;
	}
private:
	const TimerCallback callback_;
	Timestamp expiration_;	///< 有效期
	const double interval_;	///< 间隔
	const bool repeat_;		///< 是否重复
	const int64_t sequence_;	///< 序号

	static std::atomic_int64_t s_num_created_;	///< 创建次数
};

/**
 * @brief 定时器定义
 * 
 */
class TimerId {
public:
	TimerId() : timer_(nullptr), sequence_(0) {}
	TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq) {}

	friend class TimerQueue;
private:
	Timer* timer_;	///< 定时器
	int64_t sequence_;	///< 序号
};

/**
 * @brief 定时器队列
 * 
 */
class TimerQueue : noncopyable {
public:
	explicit TimerQueue(EventLoop* loop);
	~TimerQueue();

	// 添加定时器
	TimerId AddTimer(TimerCallback cb, Timestamp when, double interval = 0.0);
	// 取消定时器
	void cancel(TimerId timer_id);

	using TimeEntry = std::pair<Timestamp, Timer*>;
	using TimerList = std::set<TimeEntry>;
	using ActiveTimer = std::pair<Timer*, int64_t>;
	using ActiveTimerSet = std::set<ActiveTimer>;
private:

	void AddTimerInLoop(Timer* timer);
	void CancelInLoop(TimerId timer_id);
	void HandleRead(void);
	std::vector<TimeEntry> GetExpired(Timestamp now);
	void reset(const std::vector<TimeEntry>& expired, Timestamp now);
	bool insert(Timer* timer);

	EventLoop* loop_;
	const int timerfd_;				///< 时间描述符
	EventChannel timerfd_channel_;	///< 时间描述符通道
	TimerList timers_;				///< 定时器列表

	ActiveTimerSet activeTimers_;	///< 活动定时器
	bool calling_expired_timers_;	///< 正在调用超时定时器
	ActiveTimerSet canceling_timers_;	///< 取消的定时器
};

} // namespace net

} // namespace arss
