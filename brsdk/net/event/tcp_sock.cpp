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
 * @file tcp_sock.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2022-01-16
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "tcp_sock.hpp"

namespace brsdk {

namespace net {
	
TcpSocket::~TcpSocket() {
	close();
}

int TcpSocket::listen(void) {
	return sock_listen(sockfd_);
}

int TcpSocket::accept(Address& peeraddr) {
	sock_addr_t addr;
	int ret = sock_accept(sockfd_, addr);
	peeraddr.set_sockaddr(addr);
	return ret;
}

// 关闭写端
void TcpSocket::ShutdownWrite(void) {
	sock_shutdown(sockfd_);
}

// 立即关闭
void TcpSocket::close(void) {
	if (sockfd_ < 0) {
		return;
	}
	sock_close2(sockfd_);
	sockfd_ = -1;
}

// 获取tcp信息
bool TcpSocket::GetTcpInfo(struct tcp_info* info) const {
	socklen_t len = sizeof(*info);
	memset(info, 0, sizeof(*info));
	return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, info, &len) == 0;
}

bool TcpSocket::GetTcpInfoString(char* buf, int len) const {
	struct tcp_info tcpi;
	bool ok = GetTcpInfo(&tcpi);
    if (ok) {
        snprintf(buf, len,
                 "unrecovered=%u "
                 "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
                 "lost=%u retrans=%u rtt=%u rttvar=%u "
                 "sshthresh=%u cwnd=%u total_retrans=%u",
                 tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
                 tcpi.tcpi_rto,          // Retransmit timeout in usec
                 tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
                 tcpi.tcpi_snd_mss, tcpi.tcpi_rcv_mss,
                 tcpi.tcpi_lost,     // Lost packets
                 tcpi.tcpi_retrans,  // Retransmitted packets out
                 tcpi.tcpi_rtt,      // Smoothed round trip time in usec
                 tcpi.tcpi_rttvar,   // Medium deviation
                 tcpi.tcpi_snd_ssthresh, tcpi.tcpi_snd_cwnd,
                 tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
    }
    return ok;
}

bool TcpSocket::GetTcpInfoString(std::string& info) const {
	char buf[256] = "";
	bool ok = GetTcpInfoString(buf, sizeof(buf));
	if (ok) {
		info = buf;
	}

	return ok;
}

// 设置阻塞
void TcpSocket::SetNonblock(bool on) {
	sock_set_nonblock(sockfd_, on);
}

// 设置非延时发送
void TcpSocket::SetNodelay(bool on) {
	sock_set_nodelay(sockfd_, on);
}

// 设置地址重用
void TcpSocket::SetReuseaddr(bool on) {
	sock_set_addr_reuse(sockfd_, on);
}

// 设置端口重用
void TcpSocket::SetReusepot(bool on) {
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval,
                           static_cast<socklen_t>(sizeof optval));
    if (ret < 0 && on) {
        LOG_SYSERR << "SO_REUSEPORT failed.";
    }
#else
    if (on) {
        LOG_ERROR << "SO_REUSEPORT is not supported.";
    }
#endif
}

// 设置心跳
void TcpSocket::SetKeepalive(bool on) {
	sock_set_keepalive(sockfd_, on);
}

// 除非发送异常，否则会直到数据发送完才返回
// ssize_t writen(const void* data, size_t len);
// ssize_t readen(void* data, size_t len);

// send
ssize_t TcpSocket::send(const void* data, size_t len, int flags) {
	return sock_send(sockfd_, data, len, flags);
}

// recv
ssize_t TcpSocket::recv(void* data, size_t len, int flags) {
	return sock_recv(sockfd_, data, len, flags);
}

// write/v
ssize_t TcpSocket::write(const void* data, size_t len) {
	return sock_write(sockfd_, data, len);
}

ssize_t TcpSocket::writev(const struct iovec* iov, int iovcnt) {
	return sock_writev(sockfd_, iov, iovcnt);
}

// read/v
ssize_t TcpSocket::read(void* data, size_t len) {
	return sock_read(sockfd_, data, len);
}

ssize_t TcpSocket::readv(const struct iovec* iov, int iovcnt) {
	return sock_readv(sockfd_, iov, iovcnt);
}

} // namespace net

} // namespace brsdk
