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
 * @file tcp_sock.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2022-01-16
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include "brsdk/net/socket/socket.hpp"

namespace brsdk {

namespace net {
	
/**
 * @brief tcp套接字
 * 
 */
class TcpSocket : public Socket {
public:
	explicit TcpSocket(int sockfd) : Socket(sockfd) { }
	virtual ~TcpSocket();
public:
	// tcp监听
	int listen(void);
	int accept(Address& peeraddr);
	// 关闭写端
	void ShutdownWrite(void);
	// 立即关闭
	virtual void close(void) override;
	// 获取tcp信息
	bool GetTcpInfo(struct tcp_info* info) const;
	bool GetTcpInfoString(char* buf, int len) const;
	bool GetTcpInfoString(std::string& info) const;

	// 设置阻塞
	void SetNonblock(bool on);
	// 设置非延时发送
	void SetNodelay(bool on);
	// 设置地址重用
	void SetReuseaddr(bool on);
	// 设置端口重用
	void SetReusepot(bool on);
	// 设置心跳
	void SetKeepalive(bool on);

	// 除非发送异常，否则会直到数据发送完才返回
	// ssize_t writen(const void* data, size_t len);
	// ssize_t readen(void* data, size_t len);

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
