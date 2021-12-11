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
 * @file poller_epoll.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-30
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_loop.hpp"
#include "event_channel.hpp"
#include "poller_epoll.hpp"

#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>

// On Linux, the constants of poll(2) and epoll(4)
// are expected to be the same.
static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

namespace brsdk {

namespace net {

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop* loop)
	: EventPoller(loop),
	  epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
	  events_(kInitEventListSize) {
	if (epollfd_ < 0) {
		LOG_SYSFATAL << "EpollPoller::EpollerPoller";
	}
}
EpollPoller::~EpollPoller() {
	::close(epollfd_);
}

Timestamp EpollPoller::poll(int timeoutms, EventChanneList* activer_channels) {
	LOG_TRACE << "fd total count " << channels_.size();
	int events_num = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutms);
	int errno_back = errno;
	Timestamp now(Timestamp::now());

	if (events_num > 0) {
		LOG_TRACE << events_num << " events happened";
		FillActiveChannels(events_num, activer_channels);
		if (implicit_cast<size_t>(events_num) == events_.size()) {
			// 以两倍扩容
			events_.resize(events_.size() * 2);
		}
	} else if (events_num == 0) {
		LOG_TRACE << "nothing happened";
	} else {
		if (errno_back != EINTR) {
			errno = errno_back;
			LOG_TRACE << "EpollPoller::poll()";
		}
	}

	return now;
}

// 通道更新
void EpollPoller::UpdateChannel(EventChannel* channel) {
	EventPoller::AssertInLoopThread();
	const int index = channel->index();
	LOG_TRACE << "fd = " << channel->fd()
			  << " events = " << channel->events() << " index " << index;

	if (index == kNew || index == kDeleted) {
		int fd = channel->fd();
		if (index == kNew) {
			channels_[fd] = channel;
		} else {

		}
		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	} else {
		int fd = channel->fd();
		(void)fd;
		if (channel->IsNoneEvent()) {
			update(EPOLL_CTL_DEL, channel);
			channel->set_index(kDeleted);
		} else {
			update(EPOLL_CTL_MOD, channel);
		}
	}

}
// 通道移除
void EpollPoller::RemoveChannel(EventChannel* channel) {
	EventPoller::AssertInLoopThread();
	int fd = channel->fd();
	LOG_TRACE << "fd = " << fd;
	int index = channel->index();
	size_t n = channels_.erase(fd);
	(void)n;

	if (index == kAdded) {
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(kNew);
}

const char* EpollPoller::OperationToString(int op) {
	switch (op) {
		case EPOLL_CTL_ADD:
			return "ADD";
		case EPOLL_CTL_DEL:
			return "DEL";
		case EPOLL_CTL_MOD:
			return "MOD";
		default:
			return "Unknown Operation";
	}
}

void EpollPoller::FillActiveChannels(int num_events, EventChanneList* active_channels) {
	for (int i = 0; i < num_events; i++) {
		EventChannel* channel = static_cast<EventChannel*>(events_[i].data.ptr);
		channel->set_revents(events_[i].events);
		active_channels->push_back(channel);
	}
}
void EpollPoller::update(int operation, EventChannel* channel) {
	struct epoll_event event;
	memset(&event, 0, sizeof(event));
	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();

	LOG_TRACE << "epoll_ctl op = " << OperationToString(operation)
			  << " fd = " << fd << " event = { " << channel->EventsToString() << " }";
	
	if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
		if (EPOLL_CTL_DEL == operation) {
			LOG_SYSERR << "epoll_ctl op = " << OperationToString(operation) << " fd = " << fd;
		} else {
			LOG_SYSFATAL << "epoll_ctl op = " << OperationToString(operation) << " fd = " << fd;
		}
	}
}

} // namespace net

} // namespace brsdk
