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
 * @file event_loop_thread.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-26
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include "brsdk/lock/condition.hpp"
#include "brsdk/lock/mutex.hpp"
#include "brsdk/thread/thread.hpp"
#include "event_typedef.hpp"

namespace brsdk {

namespace net {

class EventLoopThread : noncopyable {
public:
	EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
					const std::string& name = std::string());
	~EventLoopThread();
	EventLoop* StartLoop(void);
private:
	void ThreadFunc(void);

	EventLoop* loop_ GUARDED_BY(mutex_);
	bool exiting_;							///< 退出标记
	thread::Thread thread_;					///< 线程
	MutexLock mutex_;
	Condition cond_ GUARDED_BY(mutex_);
	ThreadInitCallback callback_;
};

} // namespace net

} // namespace brsdk
