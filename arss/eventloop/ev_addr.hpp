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
 * @file ev_addr.h
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-08-31
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include "arss/net/socket_util.hpp"
#include "arss/mix/copyable.hpp"
#include <string>

namespace arss {

namespace net {

/**
 * @brief 事件循环网络地址
 * @todo 支持Unix套接字
 * 
 */
class EvInetAddress : copyable {
public:
	/**
	 * @brief Construct a new Ev Inet Address object
	 * 
	 * @param port 端口号
	 * @param loopbackOnly 仅仅用于环回
	 * @param ipv6 ipv6支持
	 */
	explicit EvInetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
	EvInetAddress(const std::string &ip, uint16_t port, bool ipv6 = false);
	EvInetAddress(const std::string &path);

	explicit EvInetAddress(const sock_addr_t &addr) : addr_(addr) {}
	explicit EvInetAddress(const struct sockaddr_in &addr);
	explicit EvInetAddress(const struct sockaddr_in6 &addr);
	explicit EvInetAddress(const struct sockaddr_un &addr);

	sa_family_t family(void) const { return addr_.sa.sa_family; }
	std::string to_ip(void) const ;
	std::string to_ipport(void) const ;
	uint16_t port(void) const ;
	// for unix
	std::string to_path(void) const ;

	sock_addr_t *get_addr(void) {
		return &addr_;
	}

	const struct sockaddr* get_sockaddr(void) const {
		return &addr_.sa;
	}

	void set_sockaddr_inet6(const struct sockaddr_in6& addr6) {
		addr_.sin6 = addr6;
	}

	uint32_t ipv4_net_endian(void) const;
	uint16_t port_net_endian(void) const;

	// DNS
	static bool resolve(const std::string& hostname, EvInetAddress *result);

	// 设置ipv6 scopeID
	void set_scope_id(uint32_t scope_id);

private:
	sock_addr_t addr_;
};

} // namespace net

} // namespace arss
