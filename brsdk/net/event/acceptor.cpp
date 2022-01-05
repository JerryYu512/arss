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
 * @file acceptor.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-24
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "acceptor.hpp"
#include "event_loop.hpp"
#include "event_channel.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace brsdk {

namespace net {

Acceptor::Acceptor(EventLoop* loop, const Address& listenAddr, bool reuseport)
	: loop_(loop),
	  accept_socket_(sock_tcp_creat(listenAddr.family())),
	  accept_channel_(loop, accept_socket_.fd()),
	  listening_(false),
	  idle_fd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
	assert(idle_fd_ >= 0);
	accept_socket_.SetReuseaddr(true);
	accept_socket_.SetNonblock(true);
	accept_socket_.BindAddress(listenAddr);
	accept_channel_.SetReadCallback(std::bind(&Acceptor::HandleRead, this));
}

Acceptor::~Acceptor() {
	accept_channel_.DisableAll();
	accept_channel_.remove();
	::close(idle_fd_);
}

void Acceptor::listen(void) {
	loop_->AssertInLoopThread();
	listening_ = true;
	accept_socket_.listen();
	accept_channel_.EnableReading();
}

void Acceptor::HandleRead(void) {
	loop_->AssertInLoopThread();
	Address peeraddr;
	int connfd = accept_socket_.accept(peeraddr);
	if (connfd >= 0) {
		LOG_TRACE << "Accepts of " << peeraddr.ipport();
		if (newConnectionCallback_) {
			newConnectionCallback_(connfd, peeraddr);
		} else {
			sock_close(connfd);
		}
	} else {
		LOG_SYSERR << "In Acceptor::HandleRead";
		if (errno == EMFILE) {
			::close(idle_fd_);
			idle_fd_ = ::accept(accept_socket_.fd(), nullptr, nullptr);
			::close(idle_fd_);
			idle_fd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
		}
	}
}

} // namespace net

} // namespace brsdk
