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
 * @file demo_ds.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2021-12-26
 * 
 * @copyright MIT License
 * 
 */
#include <stdio.h>
#include "brsdk/log/logging.hpp"
#include "brsdk/ds/array.hpp"
#include "brsdk/ds/bitmap.hpp"
#include "brsdk/ds/bits.hpp"
#include "brsdk/ds/block_queue.hpp"
#include "brsdk/ds/bounde_blocking_queue.h"
#include "brsdk/ds/darray.hpp"
#include <bitset>

using namespace brsdk;

BRSDK_ARRAY_DECL(int, demo_int_atype)
// BRSDK_DAARAY(int);

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	// 数组测试
	demo_int_atype int_at;
	demo_int_atype_init(&int_at, 1024);

	for (int i = 0; i < 1024; i++) {
		demo_int_atype_push_back(&int_at, &i);
	}

	LOG_DEBUG << "data : " << demo_int_atype_data(&int_at) << "\n";
	LOG_DEBUG << "size : " << demo_int_atype_size(&int_at) << "\n";
	LOG_DEBUG << "max size : " << demo_int_atype_maxsize(&int_at) << "\n";
	LOG_DEBUG << "empty : " << demo_int_atype_empty(&int_at) << "\n";
	LOG_DEBUG << "12 : " << *demo_int_atype_at(&int_at, 12) << "\n";
	LOG_DEBUG << "front : " << *demo_int_atype_front(&int_at) << "\n";
	LOG_DEBUG << "back : " << *demo_int_atype_back(&int_at) << "\n";

	demo_int_atype_cleanup(&int_at);

	// 位测试
	uint8_t bitt[1024] = {0};

	// ds::bitmap_zero(bitt, 1024 * 8);
	// for (int i = 0; i < 1024 * 8; i++) {
	// 	printf("%d", ds::bitmap_test_bit(bitt, i));
	// }
	// printf("\n");
	ds::bitmap_fill(bitt, 1024 * 8);
	// for (int i = 0; i < 1024 * 8; i++) {
	// 	printf("%d", ds::bitmap_test_bit(bitt, i));
	// }
	// printf("\n");

	// auto mybit = [](uint8_t* bitmap, size_t pos) -> bool {
	// 	size_t n = pos / 8;
	// 	return bitmap[n] & (1 << (pos % 8));
	// };

	ds::bitmap_clear(bitt, 0, 2);
	ds::bitmap_clear(bitt, 3, 2);
	ds::bitmap_clear(bitt, 10, 1);
	ds::bitmap_clear(bitt, 17, 8);
	ds::bitmap_clear(bitt, 36, 10);
	for (int i = 0; i < 1024 * 8; i++) {
		printf("%d", ds::bitmap_test_bit(bitt, i));
	}
	printf("\n");
	LOG_DEBUG << "count next zero : " << ds::bitmap_count_next_zero(bitt, 1024 + 8, 36) << "\n";
	LOG_DEBUG << "leading zero : " << ds::bitmap_count_leading_zero(bitt, 1024 + 8) << "\n";
	LOG_DEBUG << "first zero : " << ds::bitmap_find_first_zero(bitt, 1024 + 8) << "\n";
	LOG_DEBUG << "next zero : " << ds::bitmap_find_next_zero(bitt, 1024 + 8, 0) << "\n";
	LOG_DEBUG << "next zero : " << ds::bitmap_find_next_zero(bitt, 1024 + 8, 3) << "\n";
	for (uint8_t i = 0; i < 255; i++) {
		std::cout << "clz " << std::bitset<8>(i) << "-->" << ds::clz8((uint8_t)~i) << "\n";
	}
	// for (int i = 0; i < 1024 * 8; i++) {
	// 	printf("%d", mybit(bitt, i));
	// }
	// printf("\n");

	// bits
	ds::bits_t bits;

	ds::bits_init(&bits, bitt, sizeof(bitt));

	LOG_DEBUG << "read uint8 : " << bits_read_uint8(&bits, 7) << "\n";
	LOG_DEBUG << "read uint8 : " << bits_read_uint8(&bits, 8) << "\n";
	LOG_DEBUG << "read uint16 : " << bits_read_uint16(&bits, 16) << "\n";
	LOG_DEBUG << "read uint32 : " << bits_read_uint32(&bits, 32) << "\n";
	LOG_DEBUG << "read uint64 : " << bits_read_uint64(&bits, 64) << "\n";

	// 阻塞队列1

	ds::SyncTaskQueue<int> syncq(1024);

	for (int i = 0; i < 100; i++) {
		syncq.push(std::move(i));
	}

	LOG_DEBUG << "count : " << syncq.count() << "\n";
	LOG_DEBUG << "empty : " << syncq.empty() << "\n";
	LOG_DEBUG << "full : " << syncq.full() << "\n";

	for (int i = 0; i < 10; i++) {
		int temp = 0;
		syncq.pop(temp);
		LOG_DEBUG << "out : " << temp << "\n";
	}
	LOG_DEBUG << "count : " << syncq.count() << "\n";
	syncq.stop();
	LOG_DEBUG << "push result : " << syncq.push(65535) << "\n";

	// 阻塞队列2
	ds::BoundedBlockingQueue<int> bqueue;
	for (int i = 0; i < 100; i++) {
		bqueue.put(i);
	}

	LOG_DEBUG << "capacity : " << bqueue.capacity() << "\n";
	LOG_DEBUG << "count : " << bqueue.size() << "\n";
	LOG_DEBUG << "empty : " << bqueue.empty() << "\n";
	LOG_DEBUG << "full : " << bqueue.full() << "\n";

	for (int i = 0; i < 10; i++) {
		LOG_DEBUG << "out : " << bqueue.take() << "\n";
	}
	LOG_DEBUG << "count : " << bqueue.size() << "\n";

	// 动态数组

	

	return 0;
}
