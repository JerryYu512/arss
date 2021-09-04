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
 * @file ev_buffer.h
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-09-04
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "arss/mix/copyable.hpp"
#include "arss/mix/types.hpp"
#include "arss/net/endian.hpp"
#include <algorithm>
#include <vector>
#include <string>

#include <assert.h>
#include <string.h>

namespace arss {

namespace net {

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class EvBuffer : public copyable {

public:
	static const size_t kCheapPrepend = 8;
	static const size_t kInitialSize = 1024;

	explicit EvBuffer(size_t initial_size = kInitialSize) :
		buffer_(kCheapPrepend + kInitialSize),
		readerIdx_(kCheapPrepend),
		writerIdx_(kCheapPrepend)
	{

	}

	void swap(EvBuffer& rhs) {
		buffer_.swap(rhs.buffer_);
		std::swap(readerIdx_, rhs.readerIdx_);
		std::swap(writerIdx_, rhs.writerIdx_);
	}

	size_t readable_bytes(void) const {
		return writerIdx_ - readerIdx_;
	}

	size_t writable_bytes(void) const {
		return buffer_.size() - writerIdx_;
	}

	size_t prependable_bytes(void) const {
		return readerIdx_;
	}

private:
	char *begin(void) {
		return &*buffer_.begin();
	}

	void make_space(size_t len) {
		if (writable_bytes() + prependable_bytes() < len + kCheapPrepend) {
			buffer_.resize(writerIdx_ + len);
		} else {
			size_t readable = readable_bytes();
			std::copy(begin() + readerIdx_,
					  begin() + writerIdx_,
					  begin() + kCheapPrepend);
			readerIdx_ = kCheapPrepend;
			writerIdx_ = readerIdx_ + readable;
		}
	}

private:
	std::vector<char> buffer_;
	size_t readerIdx_;
	size_t writerIdx_;
};

} // namespace net

} // namespace arss
