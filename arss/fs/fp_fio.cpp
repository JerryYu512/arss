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
 * @file fp_fio.cpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-18
 *
 * @copyright MIT License
 *
 */
#include "fp_fio.hpp"
#include "arss/defs/defs.hpp"
#include "file_util.hpp"

namespace arss {

namespace fs {

file_dec_t fpfio_open(const char* filepath, const char* mode) {
    file_dec_t fp;
    fp.fd.fp = nullptr;
    fp.name = filepath;
    fp.type = FILE_BACKEND_FIO;

    fp.fd.fp = fopen(fp.name.c_str(), mode);

    return fp;
}

bool fpfio_valid(file_dec_t& fp) { return fp.fd.fp != nullptr; }

void fpfio_close(file_dec_t& fp) {
    if (fp.fd.fp) {
        fclose(fp.fd.fp);
        fp.fd.fp = nullptr;
    }
}

int fpfio_seek(file_dec_t& fp, off_t off, int whence) { return fseek(fp.fd.fp, off, whence); }

void fpfio_sync(file_dec_t& fp) { fflush(fp.fd.fp); }

int64_t fpfio_read(file_dec_t& fp, void* ptr, size_t len) { return fread(ptr, 1, len, fp.fd.fp); }

int64_t fpfio_write(file_dec_t& fp, const void* ptr, size_t len) {
    return fwrite(ptr, 1, len, fp.fd.fp);
}

int64_t fpfio_readall1(file_dec_t& fp, ds::Buf& buf) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    buf.resize((size_t)filesize);
    return fread(buf.base, 1, (size_t)filesize, fp.fd.fp);
}

int64_t fpfio_readall2(file_dec_t& fp, std::string& buf) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    buf.resize((size_t)filesize);
    return fread((void*)buf.data(), 1, (size_t)filesize, fp.fd.fp);
}

int64_t fpfio_readall3(file_dec_t& fp, void* buf, size_t len) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    return fread(buf, 1, std::min(len, (size_t)filesize), fp.fd.fp);
}

bool fpfio_readline(file_dec_t& fp, std::string& str) {
    str.clear();
    char ch;
    while (fread(&ch, 1, 1, fp.fd.fp)) {
        if (ch == ARSS_LF) {
            // unix: LF
            return true;
        }
        if (ch == ARSS_CR) {
            // dos: CRLF
            // read LF
            if (fread(&ch, 1, 1, fp.fd.fp) && ch != ARSS_LF) {
                // mac: CR
                fseek(fp.fd.fp, -1, SEEK_CUR);
            }
            return true;
        }
        str += ch;
    }
    return str.length() != 0;
}

int64_t fpfio_readrange(file_dec_t& fp, std::string& str, size_t from, size_t to) {
    int64_t filesize = fs_size(fp.name.c_str());
    if (filesize < 0) {
        return -1;
    }
    if (to == 0 || to >= (size_t)filesize) to = (size_t)filesize - 1;
    size_t readbytes = to - from + 1;
    str.resize(readbytes);
    fseek(fp.fd.fp, from, SEEK_SET);
    return fread((void*)str.data(), 1, readbytes, fp.fd.fp);
}

}  // namespace fs

}  // namespace arss
