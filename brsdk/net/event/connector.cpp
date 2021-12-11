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
 * @file connector.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-24
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_loop.hpp"
#include "connector.hpp"
#include "event_channel.hpp"
#include <errno.h>

namespace brsdk {

namespace net {

Connector::Connector(EventLoop* loop, const Address& server_addr)
	: loop_(loop),
	  server_addr_(server_addr),
	  connect_(false),
	  state_(kDisconnected),
	  retry_delayms_(kInitRetryDelayMs) {
	LOG_DEBUG << "constructor[" << this << "]";
}

Connector::~Connector() {
	LOG_DEBUG << "destructor[" << this << "]";
	assert(!channel_);
}

void Connector::start(void) {
	connect_ = true;
	loop_->RunInLoop(std::bind(&Connector::StartInLoop, this));
}

void Connector::restart(void) {
	loop_->AssertInLoopThread();
	set_state(kDisconnected);
	retry_delayms_ = kInitRetryDelayMs;
	connect_ = true;
	StartInLoop();
}

void Connector::stop(void) {
	connect_ = false;
	loop_->QueueInLoop(std::bind(&Connector::StopInLoop, this));
}

void Connector::StartInLoop(void) {
	loop_->AssertInLoopThread();
	assert(state_ == kDisconnected);
	if (connect_) {
		connect();
	} else {
		LOG_DEBUG << "Do not connect";
	}
}

void Connector::StopInLoop(void) {
	loop_->AssertInLoopThread();
	if (state_ == kConnecting) {
		set_state(kDisconnected);
		int sockfd = RemoveAndResetChannel();
		retry(sockfd);
	}
}
void Connector::connect(void) {
	int sockfd = sock_tcp_creat(server_addr_.family());
	sock_set_nonblock(sockfd, true);
	int ret = sock_connect(sockfd, *server_addr_.addr());
	int errno_back = (ret == 0) ? 0 : errno;
	switch (errno_back) {
	case 0:
	case EINPROGRESS:
	case EINTR:
	case EISCONN:
		connecting(sockfd);
		break;
	
	case EAGAIN:
	case EPERM:
	case EAFNOSUPPORT:
	case EALREADY:
	case EBADF:
	case EFAULT:
	case ENOTSOCK:
		LOG_SYSERR << "Connect error in Connector::StartInLoop " << errno_back;
		sock_close(sockfd);
		break;

	default:
		LOG_SYSERR << "Unexpected error in Connector::StartInLoop " << errno_back;
		sock_close(sockfd);
		break;
	}
}

void Connector::connecting(int sockfd) {
	set_state(kConnecting);
	assert(!channel_);
	channel_.reset(new EventChannel(loop_, sockfd));
	// 连接成功时调用的接口
	channel_->SetWriteCallback(std::bind(&Connector::HandleWrite, this));
	channel_->SetErrorCallback(std::bind(&Connector::HandleError, this));
}

void Connector::HandleWrite(void) {
	LOG_TRACE << "Connector::HandleWrite " << state_;

	if (state_ == kConnecting) {
		int sockfd = RemoveAndResetChannel();
		int err = sock_get_error(sockfd);
		if (err) {
			LOG_WARN << "Connector::HandlWrite - SO_ERROR = "
					 << err << " " << strerror_tl(err);
			retry(sockfd);
		} else if (sock_is_self_connect(sockfd)) {
			LOG_WARN << "Connector::HandleWrite - Self connect";
		} else {
			set_state(kConnected);
			if (connect_) {
				newConnectionCallback_(sockfd);
			} else {
				sock_close(sockfd);
			}
		}
	}
}

void Connector::HandleError(void) {
	LOG_ERROR << "Connector::HandleError state=" << state_;
	if (state_ == kConnecting) {
		int sockfd = RemoveAndResetChannel();
		int err = sock_get_error(sockfd);
		LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
		retry(sockfd);
	}
}
void Connector::retry(int sockfd) {
	sock_close(sockfd);
	set_state(kDisconnected);
	if (connect_) {
		LOG_INFO << "Connector::retry - Retry connecting to " << server_addr_.ipport()
				 << " in " << retry_delayms_ << " milliseconds.";
		loop_->RunAfter(retry_delayms_ / 1000.0,
						std::bind(&Connector::StartInLoop, shared_from_this()));
		retry_delayms_ = std::min(retry_delayms_ * 2, kMaxRetryDelayMs);
	} else {
		LOG_DEBUG << "Do not connect";
	}
}
int Connector::RemoveAndResetChannel(void) {
	channel_->DisableAll();
	channel_->remove();

	int sockfd = channel_->fd();
	loop_->QueueInLoop(std::bind(&Connector::ResetChannel, this));

	return sockfd;
}
void Connector::ResetChannel(void) {
	channel_.reset();
}

} // namespace net

} // namespace brsdk
