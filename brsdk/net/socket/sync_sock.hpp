/**
 * MIT License
 * 
 * Copyright © 2022 <Jerry.Yu>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS";, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file sync_sock.hpp
 * @brief 同步的网络连接
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2022-01-16
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "socket.hpp"

namespace brsdk {

namespace net {

class TcpSyncServerSocket : public Socket {
public:
	TcpSyncServerSocket(const Address& addr, bool addreuse = true);
	explicit TcpSyncServerSocket(int sockfd) : Socket(sockfd) { }
	virtual ~TcpSyncServerSocket();

public:
	// 监听
	int listen(void);
	int accept(const Address& peeraddr);
	// 不使用connect
	virtual int connect(const Address& peeraddr) override {
		return -1;
	}
	void shutdown(void);
	// 关闭
	virtual void close(void) override;

	// 阻塞
	void nonblock(bool on);
	// 非延时
	void nondelay(bool on);
	// 地址重用
	void reuseport(bool on);
	// 长连接
	void keepalive(bool on);
	
	// send
	ssize_t send(const void* data, size_t len, int flags);
	// recv
	ssize_t recv(void* data, size_t len, int flags);
	// write/v
	ssize_t write(const void* data, size_t len);
	ssize_t writev(const struct iovec* iov, int iovcnt);
	// read/v
	ssize_t read(void* data, size_t len);
	ssize_t readv(const struct iovec* iov, int iovcnt);
};

class TcpSyncClientSocket : public Socket {
public:
	explicit TcpSyncClientSocket(int sockfd) : Socket(sockfd) { }
	virtual ~TcpSyncClientSocket();

public:
	// 阻塞
	void nonblock(bool on);
	// 非延时
	void nondelay(bool on);
	// 地址重用
	void reuseport(bool on);
	// 长连接
	void keepalive(bool on);
	
	// send
	ssize_t send(const void* data, size_t len, int flags);
	// recv
	ssize_t recv(void* data, size_t len, int flags);
	// write/v
	ssize_t write(const void* data, size_t len);
	ssize_t writev(const struct iovec* iov, int iovcnt);
	// read/v
	ssize_t read(void* data, size_t len);
	ssize_t readv(const struct iovec* iov, int iovcnt);
};

} // namespace net

} // namespace brsdk
