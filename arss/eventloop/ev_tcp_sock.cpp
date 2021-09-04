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
 * @file ev_tcp_sock.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-09-04
 * 
 * @copyright MIT License
 * 
 */
#define CUSTOM_MODULE_NAME "net"
#include "ev_tcp_sock.hpp"
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>  // snprintf
#include "arss/log/logging.hpp"

namespace arss {

namespace net {

EvTcpSocket::~EvTcpSocket() {
	sock_close(sockfd_);
}

bool EvTcpSocket::get_tcp_info(struct tcp_info* tcpi) const {
	socklen_t len = sizeof(*tcpi);
	memset(tcpi, 0, sizeof(*tcpi));
	return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool EvTcpSocket::get_tcp_info_string(char *buf, int len) const {
	struct tcp_info tcpi;
	bool ok = get_tcp_info(&tcpi);
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

bool EvTcpSocket::get_tcp_info_string(std::string &tcpi) const {
	char buf[256] = "";
	bool ok = get_tcp_info_string(buf, sizeof(buf));
	if (ok) {
		tcpi = buf;
	}

	return ok;
}

// 地址已经被使用时抛出异常
int EvTcpSocket::bindAddress(EvInetAddress& localaddr) {
	return sock_bind(sockfd_, localaddr.to_ip().c_str(), localaddr.port());
}

// 地址已经被使用时抛出异常
int EvTcpSocket::listen(void) {
	return sock_listen(sockfd_);
}

// 成功时返回建立连接的套接字，并获取到对端地址
// 失败是返回-1
int EvTcpSocket::accept(EvInetAddress* peeraddr) {

}

// 关闭写端
void EvTcpSocket::shutdown_write(void) {
	sock_shutdown(sockfd_, 'w');
}

void EvTcpSocket::set_nodelay(bool on) {
	sock_set_nodelay(sockfd_, on);
}

void EvTcpSocket::set_reuseaddr(bool on) {
	sock_set_addr_reuse(sockfd_, on);
}

void EvTcpSocket::set_reusepot(bool on) {
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

void EvTcpSocket::set_keepalive(bool on) {
	sock_set_keepalive(sockfd_, on);
}

} // namespace net

} // namespace arss
