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
 * @file demo_net.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2021-12-26
 * 
 * @copyright MIT License
 * 
 */
#include "brsdk/log/logging.hpp"
#include "brsdk/net/inet.hpp"
#include "brsdk/net/socket/address.hpp"

using namespace brsdk;

using namespace brsdk::net;

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	std::vector<ifconfig_t> ifcs;

	ifconfig(ifcs);

	for (auto &i : ifcs) {
		LOG_DEBUG << "name : " << i.name;
		LOG_DEBUG << "ip : " << i.ip;
		LOG_DEBUG << "mask : " << i.mask;
		LOG_DEBUG << "broadcast : " << i.broadcast;
		LOG_DEBUG << "mac : " << i.mac;
	}

	Address addr1;
	Address addr2("127.0.0.1", 10000);
	Address addr3("/unixdomain/t1");
	sock_addr_t baddr1;
	baddr1.sa.sa_family = AF_INET;
	baddr1.sin.sin_port = ntohs(10001);
	inet_pton(AF_INET, "127.0.0.1", &baddr1.sin.sin_addr);

	Address addr4(baddr1);
	Address addr5(baddr1.sin);
	baddr1.sa.sa_family = AF_INET6;
	baddr1.sin6.sin6_port = ntohs(10002);
	inet_pton(AF_INET6, "::", &baddr1.sin6.sin6_addr);
	Address addr6(baddr1.sin6);
	baddr1.sa.sa_family = AF_UNIX;
	memcpy(baddr1.sun.sun_path, "/unixdomain/t2", sizeof("/unixdomain/t2"));
	Address addr7(baddr1.sun);

	LOG_DEBUG << "addr1 family : " << addr1.family();
	LOG_DEBUG << "addr1 ip : " << addr1.ip();
	LOG_DEBUG << "addr1 ipport : " << addr1.ipport();
	LOG_DEBUG << "addr1 port : " << addr1.port();

	LOG_DEBUG << "addr2 family : " << addr2.family();
	LOG_DEBUG << "addr2 ip : " << addr2.ip();
	LOG_DEBUG << "addr2 ipport : " << addr2.ipport();
	LOG_DEBUG << "addr2 port : " << addr2.port();

	LOG_DEBUG << "addr3 family : " << addr3.family();
	LOG_DEBUG << "addr3 ip : " << addr3.ip();

	LOG_DEBUG << "addr4 family : " << addr4.family();
	LOG_DEBUG << "addr4 ip : " << addr4.ip();
	LOG_DEBUG << "addr4 ipport : " << addr4.ipport();
	LOG_DEBUG << "addr4 port : " << addr4.port();

	LOG_DEBUG << "addr5 family : " << addr5.family();
	LOG_DEBUG << "addr5 ip : " << addr5.ip();
	LOG_DEBUG << "addr5 ipport : " << addr5.ipport();
	LOG_DEBUG << "addr5 port : " << addr5.port();

	LOG_DEBUG << "addr6 family : " << addr6.family();
	LOG_DEBUG << "addr6 ip : " << addr6.ip();
	LOG_DEBUG << "addr6 ipport : " << addr6.ipport();
	LOG_DEBUG << "addr6 port : " << addr6.port();

	LOG_DEBUG << "addr7 family : " << addr7.family();
	LOG_DEBUG << "addr7 ip : " << addr7.ip();

	return 0;
}
