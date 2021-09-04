/**
 * MIT License
 * 
 * Copyright © 2021 <wotsen>.
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
 * @file ev_tcp_sock.h
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-08-31
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include "arss/mix/noncopyable.hpp"
#include <netinet/tcp.h>
#include <string>
#include "ev_addr.hpp"

namespace arss {

namespace net {

/**
 * @brief tcp socket 封装
 * 
 */
class EvTcpSocket : noncopyable {
public:
	explicit EvTcpSocket(int sockfd) : sockfd_(sockfd) { }
	~EvTcpSocket();

	int fd() const { return sockfd_; }
	bool get_tcp_info(struct tcp_info*) const;
	bool get_tcp_info_string(char *buf, int len) const;
	bool get_tcp_info_string(std::string &) const;

	// 地址已经被使用时抛出异常
	int bindAddress(EvInetAddress& localaddr);
	// 地址已经被使用时抛出异常
	int listen(void);

	// 成功时返回建立连接的套接字，并获取到对端地址
	// 失败是返回-1
	int accept(EvInetAddress* peeraddr);

	// 关闭写端
	void shutdown_write(void);

	void set_nodelay(bool on);
	void set_reuseaddr(bool on);
	void set_reusepot(bool on);
	void set_keepalive(bool on);

private:
	int sockfd_;	///< 套接字
};

} // namespace net

} // namespace arss
