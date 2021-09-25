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
 * @file ev_zlib_stream.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once
#include "arss/mix/noncopyable.hpp"
#include "ev_buffer.hpp"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <zlib.h>

namespace arss {

namespace net {

// input is zlib compressed data, output uncompressed data
// FIXME: finish this
class EvZlibInputStream : noncopyable {
public:
    explicit EvZlibInputStream(EvBuffer* output) : output_(output), zerror_(Z_OK) {
        memZero(&zstream_, sizeof zstream_);
        zerror_ = inflateInit(&zstream_);
    }

    ~EvZlibInputStream() { finish(); }

    bool write(str::StringPiece buf);
    bool write(EvBuffer* input);
    bool finish();
    // inflateEnd(&zstream_);

private:
    int decompress(int flush);

    EvBuffer* output_;
    z_stream zstream_;
    int zerror_;
};

// input is uncompressed data, output zlib compressed data
class EvZlibOutputStream : noncopyable {
public:
    explicit EvZlibOutputStream(EvBuffer* output) : output_(output), zerror_(Z_OK), bufferSize_(1024) {
        memZero(&zstream_, sizeof zstream_);
        zerror_ = deflateInit(&zstream_, Z_DEFAULT_COMPRESSION);
    }

    ~EvZlibOutputStream() { finish(); }

    // Return last error message or NULL if no error.
    const char* zlibErrorMessage() const { return zstream_.msg; }

    int zlibErrorCode() const { return zerror_; }
    int64_t inputBytes() const { return zstream_.total_in; }
    int64_t outputBytes() const { return zstream_.total_out; }
    int internalOutputBufferSize() const { return bufferSize_; }

    bool write(str::StringPiece buf) {
        if (zerror_ != Z_OK) return false;

        assert(zstream_.next_in == NULL && zstream_.avail_in == 0);
        void* in = const_cast<char*>(buf.data());
        zstream_.next_in = static_cast<Bytef*>(in);
        zstream_.avail_in = buf.size();
        while (zstream_.avail_in > 0 && zerror_ == Z_OK) {
            zerror_ = compress(Z_NO_FLUSH);
        }
        if (zstream_.avail_in == 0) {
            assert(static_cast<const void*>(zstream_.next_in) == buf.end());
            zstream_.next_in = NULL;
        }
        return zerror_ == Z_OK;
    }

    // compress input as much as possible, not guarantee consuming all data.
    bool write(EvBuffer* input) {
        if (zerror_ != Z_OK) return false;

        void* in = const_cast<char*>(input->peek());
        zstream_.next_in = static_cast<Bytef*>(in);
        zstream_.avail_in = static_cast<int>(input->readable_bytes());
        if (zstream_.avail_in > 0 && zerror_ == Z_OK) {
            zerror_ = compress(Z_NO_FLUSH);
        }
        input->retrieve(input->readable_bytes() - zstream_.avail_in);
        return zerror_ == Z_OK;
    }

    bool finish() {
        if (zerror_ != Z_OK) return false;

        while (zerror_ == Z_OK) {
            zerror_ = compress(Z_FINISH);
        }
        zerror_ = deflateEnd(&zstream_);
        bool ok = zerror_ == Z_OK;
        zerror_ = Z_STREAM_END;
        return ok;
    }

private:
    int compress(int flush) {
        output_->ensureWritableBytes(bufferSize_);
        zstream_.next_out = reinterpret_cast<Bytef*>(output_->beginWrite());
        zstream_.avail_out = static_cast<int>(output_->writableBytes());
        int error = ::deflate(&zstream_, flush);
        output_->hasWritten(output_->writableBytes() - zstream_.avail_out);
        if (output_->writableBytes() == 0 && bufferSize_ < 65536) {
            bufferSize_ *= 2;
        }
        return error;
    }

    EvBuffer* output_;
    z_stream zstream_;
    int zerror_;
    int bufferSize_;
};

}  // namespace net

}  // namespace arss
