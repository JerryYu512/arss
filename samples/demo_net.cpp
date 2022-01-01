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

using namespace brsdk;

using namespace brsdk::net;

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	std::vector<ifconfig_t> ifcs;

	ifconfig(ifcs);

	for (auto &i : ifcs) {
		LOG_DEBUG << "name : " << i.name << "\n";
		LOG_DEBUG << "ip : " << i.ip << "\n";
		LOG_DEBUG << "mask : " << i.mask << "\n";
		LOG_DEBUG << "broadcast : " << i.broadcast << "\n";
		LOG_DEBUG << "mac : " << i.mac << "\n";
	}

	return 0;
}
