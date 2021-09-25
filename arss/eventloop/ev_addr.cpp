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
 * @file ev_addr.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-09-01
 * 
 * @copyright MIT License
 * 
 */
#define CUSTOM_MODULE_NAME "net"
#include "ev_addr.hpp"
#include "arss/net/endian.hpp"
#include "arss/log/logging.hpp"

#include <netdb.h>
#include <netinet/in.h>

namespace arss {

namespace net {

// INADDR_ANY use (type)value casting.
#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
static const in_addr_t kInaddrLoopback = INADDR_LOOPBACK;
#pragma GCC diagnostic error "-Wold-style-cast"

static_assert(offsetof(sockaddr_in, sin_family) == 0, "sin_family offset 0");
static_assert(offsetof(sockaddr_in6, sin6_family) == 0, "sin6_family offset 0");
static_assert(offsetof(sockaddr_in, sin_port) == 2, "sin_port offset 2");
static_assert(offsetof(sockaddr_in6, sin6_port) == 2, "sin6_port offset 2");

EvInetAddress::EvInetAddress(uint16_t port, bool loopbackOnly, bool ipv6) {
	memset(&addr_, 0, sizeof(addr_));
	if (ipv6) {
		addr_.sin6.sin6_family = AF_INET6;
		in6_addr ip = loopbackOnly ? in6addr_loopback : in6addr_any;
		addr_.sin6.sin6_addr = ip;
		addr_.sin6.sin6_port = hostToNetwork16(port);
	} else {
		addr_.sin.sin_family = AF_INET;
		in_addr_t ip = loopbackOnly ? kInaddrLoopback : kInaddrAny;
		addr_.sin.sin_addr.s_addr = ip;
		addr_.sin.sin_port = hostToNetwork16(port);
	}
}

EvInetAddress::EvInetAddress(const std::string &ip, uint16_t port, bool ipv6) {
	memset(&addr_, 0, sizeof(addr_));
	sock_set_family(&addr_, ipv6 ? AF_INET6 : AF_INET);
	sock_set_ipport(&addr_, ip.c_str(), port);
}

EvInetAddress::EvInetAddress(const std::string &path) {
	memset(&addr_, 0, sizeof(addr_));
	sock_set_family(&addr_, AF_INET);
	sock_set_path(&addr_, path.c_str());
}

EvInetAddress::EvInetAddress(const struct sockaddr_in &addr) {
	memset(&addr_, 0, sizeof(addr_));
	memcpy(&addr_.sin, &addr, sizeof(addr));
}

EvInetAddress::EvInetAddress(const struct sockaddr_in6 &addr) {
	memset(&addr_, 0, sizeof(addr_));
	memcpy(&addr_.sin6, &addr, sizeof(addr));
}

EvInetAddress::EvInetAddress(const struct sockaddr_un &addr) {
	memset(&addr_, 0, sizeof(addr_));
	memcpy(&addr_.sun, &addr, sizeof(addr));
}

std::string EvInetAddress::to_ip(void) const {
	char buf[64] = "";
	sock_addr_ip(&addr_, buf, sizeof(buf));
	return buf;
}

std::string EvInetAddress::to_ipport(void) const {
	char buf[64] = "";
	sock_addr_str(&addr_, buf, sizeof(buf));
	return buf;
}

uint16_t EvInetAddress::port(void) const {
	return sock_addr_port(&addr_);
}

std::string EvInetAddress::to_path(void) const {
	char buf[128] = "";
	sock_get_path(&addr_, buf, sizeof(buf));
	return buf;
}

uint32_t EvInetAddress::ipv4_net_endian(void) const {
    assert(family() == AF_INET);
    return addr_.sin.sin_addr.s_addr;
}

uint16_t EvInetAddress::port_net_endian(void) const {
	return addr_.sin.sin_port;
}

bool EvInetAddress::resolve(const std::string& hostname, EvInetAddress *result) {
	if (!result) {
		return false;
	}
	return 0 == sock_resolver(hostname.c_str(), &result->addr_);
}

void EvInetAddress::set_scope_id(uint32_t scope_id) {
    if (family() == AF_INET6) {
        addr_.sin6.sin6_scope_id = scope_id;
    }
}

} // namespace net

} // namespace arss
