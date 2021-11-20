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
 * @file buffer.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-17
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "arss/mix/copyable.hpp"
#include "arss/mix/types.hpp"
#include "arss/str/string_piece.hpp"
#include "arss/defs/defs.hpp"
#include "endian.hpp"
#include <algorithm>
#include <vector>
#include <string>

#include <assert.h>
#include <string.h>

namespace arss {

namespace net {

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
/// https://blog.csdn.net/qq_23542345/article/details/64129050
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class NetBuffer : copyable {

public:
	// 用于内容向前扩充，可以添加表示长度的字段
	static const size_t kCheapPrepend = 8;
	// 初始大小
	static const size_t kInitialSize = 1024;

	explicit NetBuffer(size_t initial_size = kInitialSize) :
		buffer_(kCheapPrepend + kInitialSize),
		readerIdx_(kCheapPrepend),
		writerIdx_(kCheapPrepend)
	{

	}

	void swap(NetBuffer& rhs) {
		buffer_.swap(rhs.buffer_);
		std::swap(readerIdx_, rhs.readerIdx_);
		std::swap(writerIdx_, rhs.writerIdx_);
	}

	// 可读字节数
	size_t readable_bytes(void) const {
		return writerIdx_ - readerIdx_;
	}

	// 可写字节数
	size_t writable_bytes(void) const {
		return buffer_.size() - writerIdx_;
	}

	// 前置字节数
	size_t prependable_bytes(void) const {
		return readerIdx_;
	}

	const char* peek(void) const {
		return begin() + readerIdx_;
	}

	const char* find_CRLF(void) const {
		const char temp[] = ARSS_CRLF;
		const char* crlf = std::search(peek(), begin_write(), temp, temp + 2);
		return crlf == begin_write() ? nullptr : crlf;
	}

	const char* find_CRLF(const char* start) const {
		const char temp[] = ARSS_CRLF;
		const char* crlf = std::search(start, begin_write(), temp, temp + 2);
		return crlf == begin_write() ? nullptr : crlf;
	}

	const char* find_EOL(void) const {
		const void* eol = memchr(peek(), '\n', readable_bytes());
		return static_cast<const char*>(eol);
	}

	const char* find_EOL(const char* start) const {
		const void* eol = memchr(start, '\n', readable_bytes());
		return static_cast<const char*>(eol);
	}

	void retrieve(size_t len) {
		if (len < readable_bytes()) {
			readerIdx_ += len;
		} else {
			retrieve_all();
		}
	}

	void retrieve_untill(const char* end) {
		retrieve(end - peek());
	}

	void retrieve_int64(void){
		retrieve(sizeof(int64_t));
	}

	void retrieve_int32(void){
		retrieve(sizeof(int32_t));
	}

	void retrieve_int16(void){
		retrieve(sizeof(int16_t));
	}

	void retrieve_int8(void){
		retrieve(sizeof(int8_t));
	}

	void retrieve_all(void){
		readerIdx_ = kCheapPrepend;
		writerIdx_ = kCheapPrepend;
	}

	void append(const str::StringPiece& str) {
		append(str.data(), str.size());
	}

	std::string retrieve_all_string(void) {
		return retrieve_as_string(readable_bytes());
	}

	std::string retrieve_as_string(size_t len) {
		std::string result(peek(), len);
		retrieve(len);
		return result;
	}

	str::StringPiece to_string_piece(void) const {
		return str::StringPiece(peek(), static_cast<int>(readable_bytes()));
	}

	void append(const char* data, size_t len) {
		ensure_writable_bytes(len);
		std::copy(data, data + len, begin_write());
		has_written(len);
	}

	void append(const void* data, size_t len) {
		append(static_cast<const char*>(data), len);
	}

	void ensure_writable_bytes(size_t len) {
		if (writable_bytes() < len) {
			make_space(len);
		}
	}

	char* begin_write(void) {
		return begin() + writerIdx_;
	}

	const char* begin_write(void) const {
		return begin() + writerIdx_;
	}

	void has_written(size_t len) {
		writerIdx_ += len;
	}

	void unwrite(size_t len) {
		writerIdx_ -= len;
	}

	// 网络字节序
	void append_int64(int64_t x) {
		int64_t be64 = hostToNetwork64(x);
		append(&be64, sizeof(be64));
	}

	void append_int32(int32_t x) {
		int32_t be32 = hostToNetwork32(x);
		append(&be32, sizeof(be32));
	}

	void append_int16(int16_t x) {
		int16_t be16 = hostToNetwork16(x);
		append(&be16, sizeof(be16));
	}

	void append_int8(int8_t x) {
		append(&x, sizeof(x));
	}

	int64_t read_int64(void) {
		int64_t result = peek_int64();
		retrieve_int64();
		return result;
	}

	int32_t read_int32(void) {
		int32_t result = peek_int32();
		retrieve_int32();
		return result;
	}

	int16_t read_int16(void) {
		int16_t result = peek_int16();
		retrieve_int16();
		return result;
	}

	int8_t read_int8(void) {
		int8_t result = peek_int8();
		retrieve_int8();
		return result;
	}

	int64_t peek_int64(void) const {
		int64_t be64 = 0;
		::memcpy(&be64, peek(), sizeof(be64));
		return networkToHost64(be64);
	}

	int32_t peek_int32(void) const {
		int32_t be32 = 0;
		::memcpy(&be32, peek(), sizeof(be32));
		return networkToHost32(be32);
	}

	int16_t peek_int16(void) const {
		int16_t be16 = 0;
		::memcpy(&be16, peek(), sizeof(be16));
		return networkToHost16(be16);
	}

	int8_t peek_int8(void) const {
		int8_t be8 = *peek();
		return be8;
	}

	void prepend_int64(int64_t x) {
		int64_t be64 = hostToNetwork64(x);
		prepend(&be64, sizeof(be64));
	}

	void prepend_int32(int32_t x) {
		int32_t be32 = hostToNetwork32(x);
		prepend(&be32, sizeof(be32));
	}

	void prepend_int16(int16_t x) {
		int16_t be16 = hostToNetwork16(x);
		prepend(&be16, sizeof(be16));
	}

	void prepend_int8(int8_t x) {
		prepend(&x, sizeof(x));
	}

	void prepend(const void* data, size_t len) {
		readerIdx_ -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d + len, begin() + readerIdx_);
	}

	void shrink(size_t reserve) {
		NetBuffer other;
		other.ensure_writable_bytes(readable_bytes() + reserve);
		other.append(to_string_piece());
		swap(other);
	}

	size_t internal_capacity(void) const {
		return buffer_.capacity();
	}

	/**
	 * @brief 读取数据到buffer内
	 * 
	 * @param fd fd
	 * @param savedErrno read的错误码 @c errno
	 * @return ssize_t 
	 */
	ssize_t read_fd(int fd, int* savedErrno);

private:
	char *begin(void) {
		return &*buffer_.begin();
	}

	const char *begin(void) const {
		return &*buffer_.begin();
	}

	void make_space(size_t len) {
		if (writable_bytes() + prependable_bytes() < len + kCheapPrepend) {
			// 重新分配内存
			buffer_.resize(writerIdx_ + len);
		} else {
			// 数据前移
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
