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
 * @file event_loop.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-17
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_loop.hpp"
#include "event_channel.hpp"
#include "poller.hpp"
#include "arss/net/socket/socket_util.hpp"

#include <algorithm>
// #include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

namespace arss {

namespace net {

__thread EventLoop* t_loop_in_this_thread = nullptr;

// 轮询其间隔10s
const int kPollTimeMs = 10000;

static int create_eventfd(void) {
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		LOG_SYSERR << "Failed in eventfd";
		abort();
	}

	return evtfd;
}

EventLoop* EventLoop::GetEventLoopOfCurrentThead(void) {
	return t_loop_in_this_thread;
}

EventLoop::EventLoop()
	: looping_(false),
	  quit_(false),
	  event_handling_(false),
	  calling_pending_functors_(false),
	  iteration_(0),
	  tid_(thread::tid()),
	  poller_(EventPoller::NewDefaultPoller(this)),
	  timer_queue_(new TimerQueue(this)),
	  wakeup_fd_(create_eventfd()),
	  wakeup_channel_(new EventChannel(this, wakeup_fd_)),
	  current_active_channel_(nullptr) {
	LOG_DEBUG << "EventLopp created " << this << " in thread " << tid_;

	// 全局判断，一个线程只能创建一个
	if (t_loop_in_this_thread) {
		LOG_FATAL << "Another EventLoop " << t_loop_in_this_thread
				  << " exists in this thread " << tid_;
	} else {
		t_loop_in_this_thread = this;
	}

	// 设置唤醒通道的读事件
	wakeup_channel_->SetReadCallback(std::bind(&EventLoop::HandleWakeupRead, this));
	// 始终允许读事件
	wakeup_channel_->EnableReading();
}

EventLoop::~EventLoop() {
	LOG_DEBUG << "EventLoop" << this << " of thread " << tid_
			  << " destructs in thread " << thread::tid();
	// 移除唤醒通道
	wakeup_channel_->DisableAll();
	wakeup_channel_->remove();
	::close(wakeup_fd_);
	t_loop_in_this_thread = nullptr;
}

void EventLoop::loop(void) {
	assert(!looping_);
	AssertInLoopThread();
	looping_ = true;
	quit_ = false;

	LOG_TRACE << "EventLoop " << this << " start looping";

	while (!quit_) {
		// 清空活动队列
		active_channels_.clear();
		// 轮询活动通道
		poll_return_time_ = poller_->poll(kPollTimeMs, &active_channels_);
		// loop次数增加
		++iteration_;
		if (Logger::logLevel() <= Logger::TRACE) {
			PrintActiveChannels();
		}
		// 正在处理事件
		event_handling_ = true;
		// 活动事件轮询处理
		for (EventChannel* channel : active_channels_) {
			current_active_channel_ = channel;
			current_active_channel_->HandleEvent(poll_return_time_);
		}
		current_active_channel_ = nullptr;
		// 事件处理结束
		event_handling_ = false;
		// 处理待处理的回调接口
		DoPendingFunctors();
	}

	LOG_TRACE << "EventLoop " << this << " stop looping";
	looping_ = false;
}

void EventLoop::quit(void) {
	// FIXME:非线程安全的，退出需要使用加锁来解决
	quit_ = true;
	// 如果不是loop线程内的话，唤醒一次，因为有机会将剩下的事件/回调处理完
	if (!IsInLoopThread()) {
		wakeup();
	}
}

void EventLoop::RunInLoop(EventFunctor cb) {
	if (IsInLoopThread()) {
		// loop线程内的直接调用，因为已经在loop里了
		cb();
	} else {
		// 放到队列中，在loop循环内执行
		QueueInLoop(std::move(cb));
	}
}

void EventLoop::QueueInLoop(EventFunctor cb) {
	{
		MutexLockGuard lock(mutex_);
		pending_functors_.push_back(std::move(cb));
	}

	// 如果不是在loop线程内，或者正在执行待处理回调函数，需要唤醒一次，避免处理延后
	if (!IsInLoopThread() || calling_pending_functors_) {
		wakeup();
	}
}

size_t EventLoop::queue_size(void) const {
	MutexLockGuard lock(mutex_);
	return pending_functors_.size();
}

TimerId EventLoop::RunAt(Timestamp time, TimerCallback cb) {
	// 间隔为0则不需要间隔执行
	return timer_queue_->AddTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::RunAfter(double delay, TimerCallback cb) {
	Timestamp time(addTime(Timestamp::now(), delay));
	return RunAt(time, std::move(cb));
}

TimerId EventLoop::RunEvery(double interval, TimerCallback cb) {
	Timestamp time(addTime(Timestamp::now(), interval));
	return timer_queue_->AddTimer(std::move(cb), time, interval);
}

void EventLoop::cancel(TimerId timer_id) {
	return timer_queue_->cancel(timer_id);
}

void EventLoop::UpdateChannel(EventChannel* channel) {
	// 更新是在loop线程中更新的
	assert(channel->OwnerLoop() == this);
	AssertInLoopThread();
	// 事件更新
	poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(EventChannel* channel) {
	// 更新是在loop线程中删除的
	assert(channel->OwnerLoop() == this);
	AssertInLoopThread();
	if (event_handling_) {
		assert(current_active_channel_ == channel ||
			std::find(active_channels_.begin(), active_channels_.end(), channel) == active_channels_.end());
	}
	poller_->RemoveChannel(channel);
}

bool EventLoop::HasChannel(EventChannel* channel) {
	assert(channel->OwnerLoop() == this);
	AssertInLoopThread();
	return poller_->HasChannel(channel);
}

void EventLoop::AbortNotInLoopThread(void) {
	LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
			  << " was created in threadId_ = " << tid_
			  << ", current thread id = " << thread::tid();
}

void EventLoop::wakeup(void) {
	uint64_t one = 1;
	// 通过写数据来唤醒轮询器
	ssize_t n = sock_write(wakeup_fd_, &one, sizeof(one));
	if (n != sizeof(one)) {
		LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
	}
}

void EventLoop::HandleWakeupRead(void) {
	uint64_t one = 1;
	ssize_t n = sock_read(wakeup_fd_, &one, sizeof(one));
	if (n != sizeof(one)) {
		LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
	}
}

void EventLoop::DoPendingFunctors(void) {
	std::vector<EventFunctor> functors;
	// 正在处理回调接口
	calling_pending_functors_ = true;
	{
		MutexLockGuard lock(mutex_);
		functors.swap(pending_functors_);
	}

	for (const EventFunctor& functor : functors) {
		functor();
	}
	calling_pending_functors_ = false;
}

void EventLoop::PrintActiveChannels(void) const {
	for (const EventChannel* channel : active_channels_) {
		LOG_TRACE << "{" << channel->ReventsToString() << "}";
	}
}

} // namespace net

} // namespace arss
