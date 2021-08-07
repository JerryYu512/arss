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
 * @file fp_io.cpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-18
 *
 * @copyright MIT License
 *
 */
#include "fp_io.hpp"
#include <fcntl.h>
#include <unistd.h>
#include "arss/defs/defs.hpp"
#include "file_util.hpp"

namespace arss {

namespace fs {

file_dec_t fpio_open(const char* filepath, const char* mode) {
    file_dec_t fp;
    fp.fd.fd = -1;
    fp.name = filepath;
    fp.type = FILE_BACKEND_IO;
    std::string m = mode;
    int flags = 0;

    if (m == "r") {
        flags = O_RDONLY;
    } else if (m == "r+") {
        flags = O_RDWR;
    } else if (m == "w") {
        flags = O_WRONLY;
    } else if (m == "w+") {
        flags = O_RDWR | O_TRUNC | O_CREAT;
    } else if (m == "a+") {
        flags = O_RDWR | O_APPEND;
    } else {
        return fp;
    }

    fp.fd.fd = open(fp.name.c_str(), flags, 0666);

    return fp;
}

bool fpio_valid(file_dec_t& fp) { return fp.fd.fd >= 0; }

void fpio_close(file_dec_t& fp) {
    if (fp.fd.fd >= 0) {
        close(fp.fd.fd);
        fp.fd.fd = -1;
    }
}
int fpio_seek(file_dec_t& fp, off_t off, int whence) { return lseek(fp.fd.fd, off, whence); }

void fpio_sync(file_dec_t& fp) { fsync(fp.fd.fd); }

int64_t fpio_read(file_dec_t& fp, void* ptr, size_t len) { return read(fp.fd.fd, ptr, len); }

int64_t fpio_write(file_dec_t& fp, const void* ptr, size_t len) {
    return write(fp.fd.fd, ptr, len);
}

int64_t fpio_readall1(file_dec_t& fp, ds::Buf& buf) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    buf.resize((size_t)filesize);
    return read(fp.fd.fd, buf.base, (size_t)filesize);
}

int64_t fpio_readall2(file_dec_t& fp, std::string& buf) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    buf.resize((size_t)filesize);
    return read(fp.fd.fd, (void*)buf.data(), (size_t)filesize);
}

int64_t fpio_readall3(file_dec_t& fp, void* buf, size_t len) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    return read(fp.fd.fd, buf, std::min(len, (size_t)filesize));
}

bool fpio_readline(file_dec_t& fp, std::string& str) {
    str.clear();
    char ch;
    while (read(fp.fd.fd, &ch, 1) == 1) {
        if (ch == ARSS_LF) {
            // unix: LF
            return true;
        }
        if (ch == ARSS_CR) {
            // dos: CRLF
            // read LF
            if (read(fp.fd.fd, &ch, 1) == 1 && ch != ARSS_LF) {
                // mac: CR
                lseek(fp.fd.fd, -1, SEEK_CUR);
            }
            return true;
        }
        str += ch;
    }
    return str.length() != 0;
}

int64_t fpio_readrange(file_dec_t& fp, std::string& str, size_t from, size_t to) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    if (to == 0 || to >= (size_t)filesize) to = (size_t)filesize - 1;
    size_t readbytes = to - from + 1;
    str.resize(readbytes);
    lseek(fp.fd.fd, from, SEEK_SET);
    return fread((void*)str.data(), 1, readbytes, fp.fd.fp);
}

}  // namespace fs

}  // namespace arss
