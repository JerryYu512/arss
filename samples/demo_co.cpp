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
 * @file demo_co.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2021-12-26
 * 
 * @copyright MIT License
 * 
 */
#include "brsdk/co/co.hpp"
#include "brsdk/log/logging.hpp"
#include "brsdk/thread/thread.hpp"

using namespace brsdk;

static void co_1(int a) {
	for (int i = 0; i < a; i++) {
		LOG_DEBUG << "co_1 : " << a << " - " << i << "\n";
		yield();
	}
}

void co_thread(void **arg) {
	auto sch = brsdk::sch_ref();
	CoSchedule **x = (CoSchedule**)arg;
	*x = sch;
	new_co_default(co_1, 1);
	new_co(1*1024*1024, co_1, 2);
	new_co(1*1024*1024, co_1, 3);
	new_co(1*1024*1024, co_1, 4);
	new_co(1*1024*1024, co_1, 5);
	new_co(1*1024*1024, co_1, 100);
	sch_run();
}

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	CoSchedule *sch = NULL;

	auto xx = std::bind(co_thread, (void**)&sch);
	thread::Thread cot(xx, "co_t");
	cot.start();
	cot.join(nullptr);

	sch_stop(sch);

	return 0;
}
