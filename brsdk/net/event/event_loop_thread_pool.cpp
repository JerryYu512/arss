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
 * @file event_loop_thread_pool.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-27
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_loop_thread_pool.hpp"
#include "event_loop_thread.hpp"
#include "event_loop.hpp"
#include <stdio.h>

namespace brsdk {

namespace net {

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop, const std::string& name) 
	: baseloop_(baseloop),
	  name_(name),
	  started_(false),
	  threadnum_(0),
	  next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
	baseloop_->AssertInLoopThread();

	started_ = true;

	for (int i = 0; i < threadnum_; i++) {
		std::string thread_name = name_ + std::to_string(i);
		EventLoopThread* t = new EventLoopThread(cb, thread_name);
		// 线程入队
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		// loop入队
		loops_.push_back(t->StartLoop());
	}

	// 不使用线程池，则使用主loop
	if (!threadnum_ && cb) {
		cb(baseloop_);
	}
}

EventLoop* EventLoopThreadPool::GetNextLoop(void) {
	baseloop_->AssertInLoopThread();
	// 默认使用主loop
	EventLoop* loop = baseloop_;

	if (!loops_.empty()) {
		loop = loops_[next_];
		next_++;
		// loop循环取了一轮置位
		if (implicit_cast<size_t>(next_) >= loops_.size()) {
			next_ = 0;
		}
	}

	return loop;
}

EventLoop* EventLoopThreadPool::GetLoopForHash(size_t hash) {
	baseloop_->AssertInLoopThread();

	// 默认使用主loop
	EventLoop* loop = baseloop_;

	if (!loops_.empty()) {
		loop = loops_[hash & loops_.size()];
	}

	return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::GetAllLoops(void) {
	baseloop_->AssertInLoopThread();

	if (loops_.empty()) {
		return std::vector<EventLoop*>(1, baseloop_);
	} else {
		return loops_;
	}
}

} // namespace net

} // namespace brsdk
