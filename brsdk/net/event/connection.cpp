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
 * @file connection.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-17
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "event_loop.hpp"
#include "connection.hpp"
#include "event_channel.hpp"
#include "brsdk/mix/weak_callback.hpp"
#include <errno.h>

namespace brsdk {

namespace net {

void TcpConnection::DefaultConnectionCallback(const TcpConnectionPtr& conn) {
	LOG_TRACE << conn->local_addr_.ipport() << " -> "
			  << conn->peer_addr_.ipport() << " is "
			  << (conn->connected() ? "UP" : "DOWN");
}

void TcpConnection::DefaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp t) {
	buf->retrieve_all();
}

TcpConnection::TcpConnection(EventLoop* loop, const std::string& name, int sockfd, const Address& local_addr, const Address& peer_addr)
	: loop_(CHECK_NOTNULL(loop)),
	  name_(name),
	  state_(kConnecting),
	  reading_(true),
	  socket_(new TcpSocket(sockfd)),
	  channel_(new EventChannel(loop, sockfd)),
	  local_addr_(local_addr),
	  peer_addr_(peer_addr),
	  high_water_mark_(64 * 1024 * 1024) {
	// 事件处理接口
	channel_->SetReadCallback(std::bind(&TcpConnection::HandleRead, this, _1));
	channel_->SetWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
	channel_->SetCloseCallback(std::bind(&TcpConnection::HandleClose, this));
	channel_->SetErrorCallback(std::bind(&TcpConnection::HandleError, this));
	LOG_DEBUG << "TcpConnection::constructor[" << name_ << "] at " << this << " fd=" << sockfd;
	socket_->SetKeepalive(true);
}

TcpConnection::~TcpConnection() {
	LOG_DEBUG << "TcpConnection::destructor[" << name_
			  << "] at " << this << " state=" << StateToString();
	assert(state_ == kDisconnected);
}

bool TcpConnection::GetTcpInfo(struct tcp_info* info) const {
	return socket_->GetTcpInfo(info);
}

std::string TcpConnection::GetTcpInfoString(void) {
	std::string info;
	socket_->GetTcpInfoString(info);
	return info;
}

void TcpConnection::send(const void* message, int len) {
	if (state_ == kConnected) {
		if (loop_->IsInLoopThread()) {
			SendInLoop(message, len);
		} else {
			void (TcpConnection::*fp)(const std::string &message) = &TcpConnection::SendInLoop;
			loop_->RunInLoop(std::bind(fp, this, std::string((const char*)message, len)));
		}
	}
}

void TcpConnection::send(const std::string& message) {
	if (state_ == kConnected) {
		if (loop_->IsInLoopThread()) {
			SendInLoop(message);
		} else {
			void (TcpConnection::*fp)(const std::string &message) = &TcpConnection::SendInLoop;
			loop_->RunInLoop(std::bind(fp, this, message));
		}
	}
}

void TcpConnection::send(NetBuffer* message) {
	if (state_ == kConnected) {
		if (loop_->IsInLoopThread()) {
			SendInLoop(message->peek(), message->readable_bytes());
			message->retrieve_all();
		} else {
			void (TcpConnection::*fp)(const std::string &message) = &TcpConnection::SendInLoop;
			loop_->RunInLoop(std::bind(fp, this, message->retrieve_all_string()));
		}
	}
}

// 发送
void TcpConnection::SendInLoop(const std::string& message) {
	SendInLoop(message.data(), message.size());
}

void TcpConnection::SendInLoop(const void* message, size_t len) {
	loop_->AssertInLoopThread();
	ssize_t nwrite = 0;
	size_t remaining = len;
	bool fault_error = false;
	if (state_ == kDisconnected) {
		LOG_WARN << "Disconnected, give up writing";
		return;
	}

	if (!channel_->iswriting() && output_buffer_.readable_bytes() == 0) {
		nwrite = socket_->write(message, len);
		if (nwrite >= 0) {
			// 剩下的部分
			remaining = len - nwrite;
			if (remaining == 0 && writeCompleteCallback_) {
				loop_->QueueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
			}
		} else {
			nwrite = 0;
			if (errno == EWOULDBLOCK) {
				LOG_SYSERR << "TcpConnection::SendInLoop";
				if (errno == EPIPE || errno == ECONNRESET) {
					fault_error = true;
				}
			}
		}
	}

	assert(remaining <= len);
	if (!fault_error && remaining > 0) {
		size_t oldlen = output_buffer_.readable_bytes();
		// 发送缓冲区的数据过多需要进行通知应用层，排查原因
		if ((oldlen + remaining) >= high_water_mark_
			&& oldlen < high_water_mark_
			&& highWaterMarkCallback_) {
			loop_->QueueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldlen + remaining));
		}
		// 剩下的部分放到发送缓冲区内
		output_buffer_.append(static_cast<const char*>(message) + nwrite, remaining);
		if (!channel_->iswriting()) {
			channel_->EnableWriting();
		}
	}
}

void TcpConnection::shutdown(void) {
	if (state_ == kConnected) {
		set_state(kDisconnecting);
		loop_->RunInLoop(std::bind(&TcpConnection::ShutDownInLoop, this));
	}
}

// 关闭
void TcpConnection::ShutDownInLoop(void) {
	loop_->AssertInLoopThread();
	// 有数据需要发送就不能关闭掉
	if (!channel_->iswriting()) {
		socket_->ShutdownWrite();
	}
}

void TcpConnection::ForceClose(void) {
	if (state_ == kConnected || state_ == kDisconnecting) {
		set_state(kDisconnecting);
		loop_->RunInLoop(std::bind(&TcpConnection::ForceCloseInLoop, this));
	}
}

void TcpConnection::ForceCloseInLoop(void) {
	loop_->AssertInLoopThread();
	if (state_ == kConnected || state_ == kDisconnecting) {
		HandleClose();
	}
}

void TcpConnection::ForceCloseWithDelay(double seconds) {
	if (state_ == kConnected || state_ == kDisconnecting) {
		set_state(kDisconnecting);
		loop_->RunAfter(seconds, makeWeakCallback(shared_from_this(), &TcpConnection::ForceClose));
	}
}

void TcpConnection::SetNoDelay(bool on) {
	socket_->SetNodelay(on);
}

const char* TcpConnection::StateToString(void) const {
    switch (state_) {
        case kDisconnected:
            return "kDisconnected";
        case kConnecting:
            return "kConnecting";
        case kConnected:
            return "kConnected";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}

// 开始读取
void TcpConnection::StartRead(void) {
	loop_->RunInLoop(std::bind(&TcpConnection::StartReadInLoop, this));
}

// 停止读取
void TcpConnection::StopRead(void) {
	loop_->RunInLoop(std::bind(&TcpConnection::StopReadInLoop, this));
}

// 建立先连接,accept成功时
void TcpConnection::ConnectEstablished(void) {
	loop_->AssertInLoopThread();
	assert(state_ == kConnecting);

	set_state(kConnected);
	// 建立连接后就使能读数据
	channel_->tie(shared_from_this());
	channel_->EnableReading();

	connectionCallback_(shared_from_this());
}

// 断开连接
void TcpConnection::ConnectDestroyed(void) {
	loop_->AssertInLoopThread();
	if (state_ == kConnected) {
		set_state(kDisconnected);
		channel_->DisableAll();
		connectionCallback_(shared_from_this());
	}
	// 从轮询器中删除channel
	channel_->remove();
}

void TcpConnection::HandleRead(Timestamp receive_time) {
	loop_->AssertInLoopThread();
	int errno_back = 0;
	ssize_t n = input_buffer_.read_fd(socket_->fd(), &errno_back);
	if (n > 0) {
		messageCallback_(shared_from_this(), &input_buffer_, receive_time);
	} else if (n == 0) {
		HandleClose();
	} else {
		errno = errno_back;
		LOG_SYSERR << "TcpConnection::HandleRead";
		HandleError();
	}
}

void TcpConnection::HandleWrite(void) {
	loop_->AssertInLoopThread();
	if (channel_->iswriting()) {
		ssize_t n = socket_->write(output_buffer_.peek(), output_buffer_.readable_bytes());
		if (n > 0) {
			output_buffer_.retrieve(n);
			// 数据发送结束就停止写事件
			if (output_buffer_.readable_bytes() == 0) {
				channel_->DisableWriting();
				if (writeCompleteCallback_) {
					loop_->QueueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
				}
				if (state_ == kDisconnecting) {
					ShutDownInLoop();
				}
			}
		} else {
			LOG_SYSERR << "TcpConnection::HandleWrite";
		}
	} else {
		LOG_TRACE << "Connection fd = " << socket_->fd()
				  << " is down, no more writing";
	}
}

void TcpConnection::HandleClose(void) {
	loop_->AssertInLoopThread();
	LOG_TRACE << "fd = " << channel_->fd() << " state = " << StateToString();
	assert(state_ == kConnected || state_ == kDisconnecting);
	channel_->DisableAll();

	TcpConnectionPtr guard(this);
	connectionCallback_(guard);
	closeCallback_(guard);
}

void TcpConnection::HandleError(void) {
	int err = sock_get_error(socket_->fd());
	LOG_ERROR << "TcpConnection::HandleError [" << name_
			  << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

// 接收
void TcpConnection::StartReadInLoop(void) {
	loop_->AssertInLoopThread();
	if (!reading_ || !channel_->isreading()) {
		channel_->EnableReading();
		reading_ = true;
	}
}

void TcpConnection::StopReadInLoop(void) {
	loop_->AssertInLoopThread();
	if (reading_ || channel_->isreading()) {
		channel_->disableReading();
		reading_ = false;
	}
}

} // namespace net

} // namespace brsdk
