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
 * @file socket.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-07
 * 
 * @copyright MIT License
 * 
 */
#include <stdexcept>
#include "socket_log.hpp"
#include "socket.hpp"

namespace brsdk {

namespace net {

Socket::~Socket() {
	close();
}

int Socket::family(void) const {
	return sock_family(sockfd_);
}

int Socket::BindAddress(const Address& localaddr) {
	return sock_bind(sockfd_, localaddr.ip().c_str(), localaddr.port());
}

int Socket::connect(const Address& peeraddr) {
	int ret = sock_connect(sockfd_, *peeraddr.addr());
	if (0 == ret) {
		connect_ = true;
	} else {
		connect_ = false;
	}

	return ret;
}

void Socket::close(void) {
	if (sockfd_ > 0) {
		sock_close(sockfd_);
		sockfd_ = -1;
	}
}

bool Socket::SelfConnect(void) {
	return sock_is_self_connect(sockfd_);
}

int Socket::error(void) {
	return sock_get_error(sockfd_);
}

int Socket::SetRecvTimeout(time_t t) {
	return sock_set_recv_timeout(sockfd_, t);
}

int Socket::SetSendTimeout(time_t t) {
	return sock_set_send_timeout(sockfd_, t);
}

int Socket::SetRecvBuflen(size_t len) {
	return sock_set_recv_buf_len(sockfd_, len);
}

int Socket::SetSendBuflen(size_t len) {
	return sock_set_send_buf_len(sockfd_, len);
}

Address Socket::LocalName(void) {
	sock_addr_t addr;
	sock_get_name(sockfd_, addr);
	return Address(addr);
}

Address Socket::PeerName(void) {
	if (!connect_) {
		LOG_FATAL << "socket (" << sockfd_ << ") not connect to peer.";
		throw std::runtime_error("socket not connect to peer.");
	}
	sock_addr_t addr;
	sock_get_peer_name(sockfd_, addr);
	return Address(addr);
}

UdpSocket::~UdpSocket() {}

// !!!:send/recv for connect-udp
// send
ssize_t UdpSocket::send(const void* data, size_t len, int flags) {
	return sock_send(sockfd_, data, len, flags);
}

// recv
ssize_t UdpSocket::recv(void* data, size_t len, int flags) {
	return sock_recv(sockfd_, data, len, flags);
}

// send_to
ssize_t UdpSocket::sendto(const void* data, size_t len, const sock_addr_t& addr) {
	return sock_sendto(sockfd_, data, len, addr);
}

// recv_from
ssize_t UdpSocket::recvfrom(void* data, size_t len, sock_addr_t& addr) {
	return sock_recvfrom(sockfd_, data, len, addr);
}

} // namespace net

} // namespace brsdk
