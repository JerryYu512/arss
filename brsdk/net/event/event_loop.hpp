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
 * @file event_loop.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-29
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <atomic>
#include <functional>
#include <vector>
#include "brsdk/mix/types.hpp"
#include "brsdk/lock/mutex.hpp"
#include "brsdk/thread/current_thread.hpp"
#include "brsdk/time/timestamp.hpp"
#include "event_typedef.hpp"
#include "timer.hpp"

namespace brsdk {

namespace net {

class EventLoop : noncopyable {
public:

	EventLoop();
	~EventLoop();

	// 启动loop，只能在loop线程中调用，成为loop线程
	void loop(void);

	// 退出loop
	void quit(void);

	// 轮询器返回时间
	Timestamp PollReturnTime(void) const {
		return poll_return_time_;
	}

	int64_t iteration(void) const {
		return iteration_;
	}

	// 在loop循环中去执行的回调接口
	// 如果是当前loop线程调用，则直接执行
	void RunInLoop(EventFunctor cb);

	// 放到queue里，之后由loop循环调用
	void QueueInLoop(EventCallback cb);

	size_t queue_size(void) const;

	// 在某个事件点执行
	TimerId RunAt(Timestamp time, TimerCallback cb);
	// 在某个时间后执行，单位s
	TimerId RunAfter(double delay, TimerCallback cb);
	// 间隔时间执行，单位s
	TimerId RunEvery(double interval, TimerCallback cb);
	// 取消定时器
	void cancel(TimerId timer_id);

	// !!!:内部接口
	// 唤醒poller，当没有事件时，但是有回调接口需要执行，需要唤醒loop
	void wakeup(void);
	void UpdateChannel(EventChannel* channel);
	void RemoveChannel(EventChannel* channel);
	bool HasChannel(EventChannel* channel);

	// 如果不是loop线程内则异常
	void AssertInLoopThread(void) {
		if (!IsInLoopThread()) {
			AbortNotInLoopThread();
		}
	}

	// 是否是在loop线程内
	bool IsInLoopThread(void) const {
		return tid_ == thread::tid();
	}

	// 是否正在处理事件
	bool EventHanding(void) const {
		return event_handling_;
	}

	// 设置上下文
	void set_context(const Any& context) {
		context_ = context;
	}

	const Any& get_context(void) const {
		return context_;
	}

	Any* get_mutable_conext(void) {
		return &context_;
	}

	// 获取当前线程的evnetloop
	static EventLoop* GetEventLoopOfCurrentThead(void);

private:
	// 未在loop线程中时，异常退出
	void AbortNotInLoopThread(void);
	// waked up去处理读事件
	void HandleWakeupRead(void);
	// 处理就绪函数
	void DoPendingFunctors(void);

	// 调试用
	void PrintActiveChannels(void) const;

	std::atomic_bool looping_;					///< 正在循环中
	std::atomic_bool quit_;						///< 是否退出循环
	std::atomic_bool event_handling_;			///< 事件处理中
	std::atomic_bool calling_pending_functors_;	///< 就绪函数调用中
	int64_t iteration_;
	const pid_t tid_;
	Timestamp poll_return_time_;					///< 轮询器返回时间
	std::unique_ptr<EventPoller> poller_;			///< 轮询器
	std::unique_ptr<TimerQueue> timer_queue_;		///< 定时器队列
	int wakeup_fd_;									///< 唤醒描述符，loop自己使用，主要是没有事件发生，但是又有回调函数需要执行时需要唤醒
	std::unique_ptr<EventChannel> wakeup_channel_;
	Any context_;

	EventChanneList active_channels_;		///< 活动通道
	EventChannel* current_active_channel_;	///< 当前活动通道

	mutable MutexLock mutex_;
	std::vector<EventFunctor> pending_functors_ GUARDED_BY(mutex_);
};

} // namespace net

} // namespace brsdk
