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
 * @file event_loop_thread.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-26
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_loop_thread.hpp"
#include "event_loop.hpp"

namespace brsdk {

namespace net {

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
	: loop_(nullptr), 
	  exiting_(false),
	  thread_(std::bind(&EventLoopThread::ThreadFunc, this), name),
	  mutex_(),
	  cond_(mutex_),
	  callback_(cb) {
}

EventLoopThread::~EventLoopThread() {
	exiting_ = true;
	if (loop_) {
		// 先等loop退出，使用完后loop_会被置空
		loop_->quit();
		// 必须放在最后，否则可能导致loop_为空指针
		thread_.join();
	}
}

EventLoop* EventLoopThread::StartLoop(void) {
	assert(!thread_.started());
	thread_.start();

	EventLoop* loop = nullptr;
	{
		MutexLockGuard lock(mutex_);
		// 等待线程启动
		while (!loop_ && !exiting_) {
			cond_.wait();
		}

		if (!exiting_) {
			loop = loop_;
		} else {
			// 如果线程刚启动就给退出了，直接返回空
			return nullptr;
		}
	}

	return loop;
}

void EventLoopThread::ThreadFunc(void) {
	// loop在线程栈里边
	EventLoop loop;

	if (callback_) {
		callback_(&loop);
	}

	{
		MutexLockGuard lock(mutex_);
		// 将栈里的loop给到成员变量
		loop_ = &loop;
		// 通知线程已经启动，loop已经创建
		cond_.notify();
	}

	// 进入循环直到退出
	loop.loop();
	MutexLockGuard lock(mutex_);
	exiting_ = true;
	loop_ = NULL;
}

} // namespace net

} // namespace brsdk
