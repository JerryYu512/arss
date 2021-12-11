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
 * @file event_channel.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-17
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_channel.hpp"
#include "event_loop.hpp"
#include <sstream>

namespace brsdk {

namespace net {

EventChannel::EventChannel(EventLoop* loop, int fd)
	: loop_(loop),
	  fd_(fd),
	  events_(0),
	  revents_(0),
	  index_(-1),
	  log_hup_(true),
	  tied_(false),
	  event_handling_(false),
	  added_to_loop_(false) {
}

EventChannel::~EventChannel() {
	assert(!event_handling_);
	assert(!added_to_loop_);
	if (loop_->IsInLoopThread()) {
		assert(!loop_->HasChannel(this));
	}
}

void EventChannel::tie(const std::shared_ptr<void>& obj) {
	tie_ = obj;
	tied_ = true;
}

void EventChannel::update(void) {
	// 已经添加到loop
	added_to_loop_ = true;
	// 更新到轮询器
	loop_->UpdateChannel(this);
}

void EventChannel::remove(void) {
	assert(IsNoneEvent());
	added_to_loop_ = false;
	loop_->RemoveChannel(this);
}

void EventChannel::HandleEvent(Timestamp receive_time) {
	std::shared_ptr<void> guard;
	if (tied_) {
		// 弱引用，需要先锁定，避免释放
		guard = tie_.lock();
		if (guard) {
			HandleEventWithGuard(receive_time);
		}
	} else {
		HandleEventWithGuard(receive_time);
	}
}

void EventChannel::HandleEventWithGuard(Timestamp receive_time) {
	// 正在处理事件
	event_handling_ = true;
	LOG_TRACE << ReventsToString();
	// 关闭链接
	if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
		if (log_hup_) {
			LOG_WARN << "fd = " << fd_ << " EventChannel::HandEvent() POLLHUP";
		}
		if (closeCallback_) closeCallback_();
	}

    if (revents_ & POLLNVAL) {
        LOG_WARN << "fd = " << fd_ << " Channel::handle_event() POLLNVAL";
    }

	// 错误
    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }
	// 有数据可读
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) readCallback_(receive_time);
    }
	// 可写事件
    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }

    event_handling_ = false;
}

std::string EventChannel::ReventsToString() const {
	return EventsToString(fd_, revents_);
}

std::string EventChannel::EventsToString(void) const {
	return EventsToString(fd_, events_);
}

std::string EventChannel::EventsToString(int fd, int ev) {
	std::ostringstream oss;

	oss << fd << ": ";
	if (ev & POLLIN) {
		oss << "IN ";
	}

	if (ev & POLLPRI) {
		oss << "PRI ";
	}

	if (ev & POLLOUT) {
		oss << "OUT ";
	}

	if (ev & POLLHUP) {
		oss << "HUP ";
	}

	if (ev & POLLRDHUP) {
		oss << "RDHUP ";
	}

	if (ev & POLLERR) {
		oss << "ERR ";
	}

	if (ev & POLLNVAL) {
		oss << "NVAL ";
	}

	return oss.str();
}

} // namespace net

} // namespace brsdk
