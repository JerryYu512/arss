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
 * @file demo_atomic.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2021-12-26
 * 
 * @copyright MIT License
 * 
 */
#include "brsdk/atomic/atomic.hpp"
#include "brsdk/log/logging.hpp"

using namespace brsdk;

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	int i1 = 0;

	LOG_DEBUG << "i1 = " << i1 << "\n";
	atomic_inc(&i1);
	LOG_DEBUG << "i1 = " << i1 << "\n";
	atomic_dec(&i1);
	LOG_DEBUG << "i1 = " << i1 << "\n";
	atomic_add(&i1, 5);
	LOG_DEBUG << "i1 = " << i1 << "\n";
	atomic_sub(&i1, 3);
	LOG_DEBUG << "i1 = " << i1 << "\n";
	int i1f = atomic_fetch_inc(&i1);
	LOG_DEBUG << "i1 = " << i1 << ", i1f = " << i1f << "\n";
	i1f = atomic_fetch_dec(&i1);
	LOG_DEBUG << "i1 = " << i1 << ", i1f = " << i1f << "\n";

	int i2 = 2;
	int i3 = 19;

	atomic_compare_swap(&i1, i2, i3);
	LOG_DEBUG << "i1 = " << i1 << "\n";

	return 0;
}
