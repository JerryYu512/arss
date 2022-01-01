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
 * @file poller_poll.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-31
 * 
 * @copyright MIT License
 * 
 */
#include "poller_poll.hpp"
#include "event_log.hpp"
#include "event_channel.hpp"
#include "event_loop.hpp"
#include <assert.h>
#include <errno.h>
#include <poll.h>

namespace brsdk {

namespace net {

PollPoller::PollPoller(EventLoop* loop) : EventPoller(loop) {

}

PollPoller::~PollPoller() = default;

Timestamp PollPoller::poll(int timeoutms, EventChanneList* activer_channels) {
	int evnets_num = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutms);
	int errno_back = errno;

	Timestamp now(Timestamp::now());
	if (evnets_num > 0) {
		LOG_TRACE << evnets_num << " events happened";
		FillActiveChannels(evnets_num, activer_channels);
	} else if (evnets_num == 0) {
		LOG_TRACE << " nothing happened";
	} else {
		if (errno_back != EINTR) {
			errno = errno_back;
			LOG_SYSERR << "PollPoller::poll()";
		}
	}

	return now;
}
// 通道更新
void PollPoller::UpdateChannel(EventChannel* channel) {
	EventPoller::AssertInLoopThread();
	LOG_TRACE << "fd = " << channel->fd() << " events = " << channel->events();
	
	if (channel->index() < 0) {
		struct pollfd pfd;

		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size() - 1);
		channel->set_index(idx);
		channels_[pfd.fd] = channel;
	} else {
		int idx = channel->index();
		struct pollfd& pfd = pollfds_[idx];
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if (channel->IsNoneEvent()) {
			pfd.fd = - channel->fd() - 1;
		}
	}
}
// 通道移除
void PollPoller::RemoveChannel(EventChannel* channel) {
	EventPoller::AssertInLoopThread();
	LOG_TRACE << "fd = " << channel->fd();
	int idx = channel->index();
	const struct pollfd& pfd = pollfds_[idx];
	(void)pfd;
	size_t n = channels_.erase(channel->fd());
	(void)n;
	if (implicit_cast<size_t>(idx) == pollfds_.size() - 1) {
		pollfds_.pop_back();
	} else {
		int channel_at_end = pollfds_.back().fd;
		iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
		if (channel_at_end < 0) {
			channel_at_end = - channel_at_end - 1;
		}
		channels_[channel_at_end]->set_index(idx);
		pollfds_.pop_back();
	}
}

void PollPoller::FillActiveChannels(int num_events, EventChanneList* active_channels) {
	for (auto pfd = pollfds_.begin(); pfd != pollfds_.end() && num_events > 0; pfd++) {
		if (pfd->revents > 0) {
			--num_events;
			auto ch = channels_.find(pfd->fd);
			EventChannel* channel = ch->second;
			channel->set_revents(pfd->revents);
			active_channels->push_back(channel);
		}
	}
}

} // namespace net

} // namespace brsdk
