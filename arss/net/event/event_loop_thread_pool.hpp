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
 * @file event_loop_thread_pool.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-25
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "arss/mix/noncopyable.hpp"
#include "event_typedef.hpp"

namespace arss {

namespace net {
	
class EventLoopThreadPool : noncopyable {
public:
	// baseloop为线程池的主loop，acceptor都在主loop中，用户层添加新的也在主loop中
	// FIXME:用户tcp客户端/udp如何利用线程池
	EventLoopThreadPool(EventLoop* baseloop, const std::string& name);
	~EventLoopThreadPool();

	void set_threadnum(int num) {
		threadnum_ = num;
	}

	void start(const ThreadInitCallback& cb = ThreadInitCallback());

	// 循序取一个loop(应该由baseLoop里的事件处理调用)
	EventLoop* GetNextLoop(void);
	// 随机取一个loop
	EventLoop* GetLoopForHash(size_t hash);
	// 获取全部loop
	std::vector<EventLoop*> GetAllLoops(void);

	bool started(void) const {
		return started_;
	}

	const std::string& name(void) const {
		return name_;
	}

private:
	EventLoop* baseloop_;	///< 主loop
	std::string name_;		///< 名称
	bool started_;			///< 开始标记
	int threadnum_;			///< 线程数
	int next_;				///< 下个EventLoop的编号，一个loop对应于一个线程
	std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;
};

} // namespace net

} // namespace arss
