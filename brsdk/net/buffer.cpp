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
 * @file buffer.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-17
 * 
 * @copyright MIT License
 * 
 */
#include "buffer.hpp"
#include <errno.h>
#include <sys/uio.h>
#include "socket/socket_util.hpp"

namespace brsdk {

namespace net {

const size_t NetBuffer::kCheapPrepend;
const size_t NetBuffer::kInitialSize;

ssize_t NetBuffer::read_fd(int fd, int* savedErrno) {
	char extrabuf[65536] = {0};
	struct iovec vec[2];
	const size_t writable = writable_bytes();

	vec[0].iov_base = (void*)(begin() + writerIdx_);
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof(extrabuf);

	// 空间足够则使用buffer，否则加上额外内存
	const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
	const ssize_t n = sock_readv(fd, vec, iovcnt);

	if (n < 0) {
		*savedErrno = errno;
	} else if (static_cast<size_t>(n) <= writable) {
		writerIdx_ += n;
	} else {
		writerIdx_ = buffer_.size();
		append(extrabuf, n - writable);
	}

	return n;
}

} // namespace net

} // namespace brsdk
