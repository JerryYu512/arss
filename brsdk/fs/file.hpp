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
 * @file file.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <stdexcept>
#include "brsdk/ds/buf.hpp"
#include "brsdk/mix/noncopyable.hpp"
#include "brsdk/str/string_piece.hpp"
#include "file_def.hpp"

namespace brsdk {

namespace fs {

// 文件对象
class File {
private:
    typedef file_dec_t Fp;
    struct Fop {
        Fp (*open)(const char* filepath, const char* mode);
        bool (*valid)(Fp& fp);
        void (*close)(Fp& fp);
        int (*seek)(Fp& fp, off_t off, int whence);
        void (*sync)(Fp& fp);
        int64_t (*read)(Fp& fp, void* ptr, size_t len);
        int64_t (*write)(Fp& fp, const void* ptr, size_t len);
        int64_t (*readall1)(Fp& fp, ds::Buf& buf);
        int64_t (*readall2)(Fp& fp, std::string& buf);
        int64_t (*readall3)(Fp& fp, void* buf, size_t len);
        bool (*readline)(Fp& fp, std::string& str);
        int64_t (*readrange)(Fp& fp, std::string& str, size_t from, size_t to);
    };

    static Fop fops[FILE_BACKEND_NUM];

public:
    File(file_backend_type type = FILE_BACKEND_FIO) {
        if (type >= FILE_BACKEND_NUM) {
            throw std::invalid_argument("error file backend type");
        }
        fp_.type = type;
        op_ = &File::fops[type];
        if (fp_.type == FILE_BACKEND_FIO) {
            fp_.fd.fp = nullptr;
        } else {
            fp_.fd.fd = -1;
        }
    }

    ~File() { close(); }

    file_dec_t* get_fd() { return &fp_; }

    int open(const char* filepath, const char* mode) {
        close();
        if (!filepath) {
            return -1;
        }
        fp_ = op_->open(filepath, mode);
        if (fp_.type == FILE_BACKEND_FIO) {
            return fp_.fd.fp ? 0 : -1;
        } else {
            return fp_.fd.fd >= 0 ? 0 : -1;
        }
    }

    void close() { op_->close(fp_); }

    int seek(off_t off, int whence) { return op_->seek(fp_, off, whence); }

    void sync() { op_->sync(fp_); }

    size_t read(void* ptr, size_t len) { return op_->read(fp_, ptr, len); }

    size_t write(const void* ptr, size_t len) { return op_->write(fp_, ptr, len); }

    size_t size() {
        struct stat st;
        memset(&st, 0, sizeof(st));
        stat(fp_.name.c_str(), &st);
        return st.st_size;
    }

    size_t readall(ds::Buf& buf) {
        auto ret = op_->readall1(fp_, buf);
        return ret < 0 ? 0 : (size_t)ret;
    }

    size_t readall(std::string& str) {
        auto ret = op_->readall2(fp_, str);
        return ret < 0 ? 0 : (size_t)ret;
    }

    size_t readall(void* ptr, size_t len) {
        auto ret = op_->readall3(fp_, ptr, len);
        return ret < 0 ? 0 : (size_t)ret;
    }

    bool readline(std::string& str) { return op_->readline(fp_, str); }

    int readrange(std::string& str, size_t from = 0, size_t to = 0) {
        int64_t ret = op_->readrange(fp_, str, from, to);
        return ret < 0 ? 0 : static_cast<int>(ret);
    }

private:
    Fp fp_;
    Fop* op_;
};

// read small file < 64KB
class ReadSmallFile : public brsdk::noncopyable {
public:
    ReadSmallFile(brsdk::str::StringArg filename);
    ~ReadSmallFile();

    // return errno
    template <typename String>
    int readToString(int maxSize, String* content, int64_t* fileSize, int64_t* modifyTime,
                     int64_t* createTime);

    /// Read at maxium kBufferSize into buf_
    // return errno
    int readToBuffer(int* size);

    const char* buffer() const { return buf_; }

    static const int kBufferSize = 64 * 1024;

private:
    int fd_;
    int err_;
    char buf_[kBufferSize];
};

// read the file content, returns errno if error happens.
template <typename String>
int readFile(brsdk::str::StringArg filename, int maxSize, String* content, int64_t* fileSize = NULL,
             int64_t* modifyTime = NULL, int64_t* createTime = NULL) {
    ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

// not thread safe
class AppendFile : public brsdk::noncopyable {
public:
    explicit AppendFile(brsdk::str::StringArg filename);

    ~AppendFile();

    void append(const char* logline, size_t len);

    void flush();

    off_t writtenBytes() const { return writtenBytes_; }

private:
    size_t write(const char* logline, size_t len);

    FILE* fp_;
    char buffer_[64 * 1024];
    off_t writtenBytes_;
};

}  // namespace fs

}  // namespace brsdk
