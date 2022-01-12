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
 * @file address.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-30
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "brsdk/net/socket/socket_util.hpp"
#include <string>

namespace brsdk {

namespace net {

/**
 * @brief 网络地址
 * 
 */
class Address {
public:
	/**
	 * @brief Construct a new Address object
	 * 
	 * @param port 端口号
	 * @param loopback 用于环回
	 * @param ipv6 支持ipv6
	 */
	explicit Address(uint16_t port = 0, bool loopback = false, bool ipv6 = false);
	// 适用于ipv4/ipv6
	Address(const std::string &ip, uint16_t port, bool ipv6 = false);
	// 适用于unix
	Address(const std::string &path);

	explicit Address(const sock_addr_t &addr) : addr_(addr) {}
	explicit Address(const sockaddr_in &addr);
	explicit Address(const sockaddr_in6 &addr);
	explicit Address(const sockaddr_un &addr);

	// 取地址族
	sa_family_t family(void) const { return addr_.sa.sa_family; }

	// 获取ip
	std::string ip(void) const;

	// 获取ip:port
	std::string ipport(void) const;

	// 端口号
	uint16_t port(void) const;

	// unix path
	std::string path(void) const {
		return ip();
	}

	///< 地址引用
	const sock_addr_t *addr(void) const {
		return &addr_;
	}

	///< 通用地址段
	const struct sockaddr* sockaddr(void) const {
		return &addr_.sa;
	}

	// 设置ipv6
	void set_sockaddr(const sock_addr_t& addr) {
		addr_ = addr;
	}

	// 设置ipv6
	void set_sockaddr_inet6(const struct sockaddr_in6& addr6) {
		addr_.sin6 = addr6;
	}

	// 设置ipv4
	void set_sockaddr_inet4(const struct sockaddr_in& addr) {
		addr_.sin = addr;
	}

	// 设置unix
	void set_sockaddr_unix(const struct sockaddr_un& un) {
		addr_.sun = un;
	}

	// 设置ipv6 scopeID
	void SetScopeId(uint32_t scope_id);

	// 获取ipv4网络字节序
	uint32_t Ipv4NetEndian(void) const;

	// 端口号网络字节序
	uint16_t PortNetEndian(void) const;

	// dns
	static bool resolve(const std::string& hostname, Address& addr);

private:
	sock_addr_t addr_;	// 地址
};
	
} // namespace net

} // namespace brsdk
