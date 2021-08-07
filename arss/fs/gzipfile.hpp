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
 * @file gzipfile.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-17
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <zlib.h>
#include "arss/mix/noncopyable.hpp"
#include "arss/str/string_piece.hpp"

namespace arss {

namespace fs {

class GzipFile : noncopyable {
public:
    GzipFile(GzipFile&& rhs) noexcept : file_(rhs.file_) { rhs.file_ = NULL; }

    ~GzipFile() {
        if (file_) {
            ::gzclose(file_);
        }
    }

    GzipFile& operator=(GzipFile&& rhs) noexcept {
        swap(rhs);
        return *this;
    }

    bool valid() const { return file_ != NULL; }
    void swap(GzipFile& rhs) { std::swap(file_, rhs.file_); }
#if ZLIB_VERNUM >= 0x1240
    bool setBuffer(int size) { return ::gzbuffer(file_, size) == 0; }
#endif

    // return the number of uncompressed bytes actually read, 0 for eof, -1 for error
    int read(void* buf, int len) { return ::gzread(file_, buf, len); }

    // return the number of uncompressed bytes actually written
    int write(str::StringPiece buf) { return ::gzwrite(file_, buf.data(), buf.size()); }

    // number of uncompressed bytes
    off_t tell() const { return ::gztell(file_); }

#if ZLIB_VERNUM >= 0x1240
    // number of compressed bytes
    off_t offset() const { return ::gzoffset(file_); }
#endif

    // int flush(int f) { return ::gzflush(file_, f); }

    static GzipFile openForRead(str::StringArg filename) {
        return GzipFile(::gzopen(filename.c_str(), "rbe"));
    }

    static GzipFile openForAppend(str::StringArg filename) {
        return GzipFile(::gzopen(filename.c_str(), "abe"));
    }

    static GzipFile openForWriteExclusive(str::StringArg filename) {
        return GzipFile(::gzopen(filename.c_str(), "wbxe"));
    }

    static GzipFile openForWriteTruncate(str::StringArg filename) {
        return GzipFile(::gzopen(filename.c_str(), "wbe"));
    }

private:
    explicit GzipFile(gzFile file) : file_(file) {}

    gzFile file_;
};

} // namespace fs

} // namespace arss
