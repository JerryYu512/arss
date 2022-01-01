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
 * @file file.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "file.hpp"
#include "brsdk/log/logging.hpp"
#include "fp_fio.hpp"
#include "fp_io.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

namespace brsdk {

namespace fs {

File::Fop File::fops[FILE_BACKEND_NUM] = {
    {
        fpio_open,
        fpio_valid,
        fpio_close,
        fpio_seek,
        fpio_sync,
        fpio_read,
        fpio_write,
        fpio_readall1,
        fpio_readall2,
        fpio_readline,
        fpio_readrange,
    },
    {
        fpfio_open,
        fpfio_valid,
        fpfio_close,
        fpfio_seek,
        fpfio_sync,
        fpfio_read,
        fpfio_write,
        fpfio_readall1,
        fpfio_readall2,
        fpfio_readline,
        fpfio_readrange,
    },
};

AppendFile::AppendFile(brsdk::str::StringArg filename)
    : fp_(::fopen(filename.c_str(), "ae")),  // 'e' for O_CLOEXEC
      writtenBytes_(0) {
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof buffer_);
    // posix_fadvise POSIX_FADV_DONTNEED ?
}

AppendFile::~AppendFile() { ::fclose(fp_); }

void AppendFile::append(const char* logline, const size_t len) {
    size_t written = 0;

    while (written != len) {
        size_t remain = len - written;
        size_t n = write(logline + written, remain);
        if (n != remain) {
            int err = ferror(fp_);
            if (err) {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror_tl(err));
                break;
            }
        }
        written += n;
    }

    writtenBytes_ += written;
}

void AppendFile::flush() { ::fflush(fp_); }

size_t AppendFile::write(const char* logline, size_t len) {
    // #undef fwrite_unlocked
    return ::fwrite_unlocked(logline, 1, len, fp_);
}

ReadSmallFile::ReadSmallFile(brsdk::str::StringArg filename)
    : fd_(::open(filename.c_str(), O_RDONLY | O_CLOEXEC)), err_(0) {
    buf_[0] = '\0';
    if (fd_ < 0) {
        err_ = errno;
    }
}

ReadSmallFile::~ReadSmallFile() {
    if (fd_ >= 0) {
        ::close(fd_);  // FIXME: check EINTR
    }
}

// return errno
template <typename String>
int ReadSmallFile::readToString(int maxSize, String* content, int64_t* fileSize,
                                          int64_t* modifyTime, int64_t* createTime) {
    static_assert(sizeof(off_t) == 8, "_FILE_OFFSET_BITS = 64");
    assert(content != NULL);
    int err = err_;
    if (fd_ >= 0) {
        content->clear();

        if (fileSize) {
            struct stat statbuf;
            if (::fstat(fd_, &statbuf) == 0) {
                if (S_ISREG(statbuf.st_mode)) {
                    *fileSize = statbuf.st_size;
                    content->reserve(
                        static_cast<int>(std::min(implicit_cast<int64_t>(maxSize), *fileSize)));
                } else if (S_ISDIR(statbuf.st_mode)) {
                    err = EISDIR;
                }
                if (modifyTime) {
                    *modifyTime = statbuf.st_mtime;
                }
                if (createTime) {
                    *createTime = statbuf.st_ctime;
                }
            } else {
                err = errno;
            }
        }

        while (content->size() < implicit_cast<size_t>(maxSize)) {
            size_t toRead =
                std::min(implicit_cast<size_t>(maxSize) - content->size(), sizeof(buf_));
            ssize_t n = ::read(fd_, buf_, toRead);
            if (n > 0) {
                content->append(buf_, n);
            } else {
                if (n < 0) {
                    err = errno;
                }
                break;
            }
        }
    }
    return err;
}

int ReadSmallFile::readToBuffer(int* size) {
    int err = err_;
    if (fd_ >= 0) {
        ssize_t n = ::pread(fd_, buf_, sizeof(buf_) - 1, 0);
        if (n >= 0) {
            if (size) {
                *size = static_cast<int>(n);
            }
            buf_[n] = '\0';
        } else {
            err = errno;
        }
    }
    return err;
}

template int readFile(brsdk::str::StringArg filename, int maxSize, std::string* content, int64_t*,
                                int64_t*, int64_t*);

template int ReadSmallFile::readToString(int maxSize, std::string* content, int64_t*,
                                                   int64_t*, int64_t*);

} // namespace fs

} // namespace brsdk
