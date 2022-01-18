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
 * @file socket.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-25
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <netinet/tcp.h>
#include "address.hpp"

namespace brsdk {

namespace net {

/**
 * @brief 套接字
 * 
 */
class Socket {
public:
	explicit Socket(int sockfd) : sockfd_(sockfd), connect_(false) {}
	virtual ~Socket();

	// common interface
	virtual int fd(void) const { return sockfd_; }
	// 地址族
	virtual int family(void) const;
	// 地址绑定
	virtual int BindAddress(const Address& localaddr);
	// 连接
	virtual int connect(const Address& peeraddr);
	// 关闭连接
	virtual void close(void);

	// 是否是连接到自身
	virtual bool SelfConnect(void);
	// 错误码
	virtual int error(void);

	// 设置接收超时时间
	virtual int SetRecvTimeout(time_t t);
	// 设置发送超时时间
	virtual int SetSendTimeout(time_t t);
	// 设置接收buf大小
	virtual int SetRecvBuflen(size_t t);
	// 设置发送buf大小
	virtual int SetSendBuflen(size_t t);
	// 本地绑定地址
	virtual Address LocalName(void);
	// 对端地址, tcp/udp-connect only
	virtual Address PeerName(void);

protected:
	int sockfd_;	///< 套接字
	bool connect_;	///< 是否连接
};

/**
 * @brief udp
 * 
 */
class UdpSocket : public Socket {
public:
	explicit UdpSocket(int sockfd) : Socket(sockfd) {}
	virtual ~UdpSocket();
public:
	// !!!:send/recv for connect-udp
	// send
	ssize_t send(const void* data, size_t len, int flags = 0);
	// recv
	ssize_t recv(void* data, size_t len, int flags = 0);

	// send_to
	ssize_t sendto(const void* data, size_t len, const sock_addr_t& addr);
	// recv_from
	ssize_t recvfrom(void* data, size_t len, sock_addr_t& addr);
};

} // namespace net

} // namespace brsdk
