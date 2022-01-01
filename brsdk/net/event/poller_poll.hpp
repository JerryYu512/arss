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
 * @file poller_poll.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-30
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "poller.hpp"
#include <vector>
#include <poll.h>

namespace brsdk {

namespace net {

class PollPoller : public EventPoller {
public:
	PollPoller(EventLoop* loop);
	~PollPoller() override;

	Timestamp poll(int timeoutms, EventChanneList* activer_channels);
	// 通道更新
	virtual void UpdateChannel(EventChannel* channel) override;
	// 通道移除
	virtual void RemoveChannel(EventChannel* channel) override;

private:
	void FillActiveChannels(int num_events, EventChanneList* active_channels);

	using PollFdList = std::vector<struct pollfd>;
	PollFdList pollfds_;
};

} // namespace net

} // namespace brsdk
