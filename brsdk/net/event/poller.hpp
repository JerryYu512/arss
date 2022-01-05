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
 * @file poller.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-07
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <map>
#include <vector>
#include "brsdk/time/timestamp.hpp"
#include "brsdk/mix/noncopyable.hpp"
#include "event_loop.hpp"
#include "event_typedef.hpp"

namespace brsdk {

namespace net {

class EventPoller : noncopyable {
public:
	EventPoller(EventLoop* loop);
	virtual ~EventPoller();

	virtual Timestamp poll(int timeout_ms, EventChanneList* active_channels) = 0;
	// 通道更新
	virtual void UpdateChannel(EventChannel* channel) = 0;
	// 通道移除
	virtual void RemoveChannel(EventChannel* channel) = 0;
	// 是否有通道
	virtual bool HasChannel(EventChannel* channel) const;
	// 创建默认轮询器
	static EventPoller* NewDefaultPoller(EventLoop* loop);
	void AssertInLoopThread(void) const {
		loop_->AssertInLoopThread();
	}
protected:
	EventChannelMap channels_;	///< 通道映射

private:
	EventLoop* loop_;
};

} // namespace net

} // namespace brsdk
