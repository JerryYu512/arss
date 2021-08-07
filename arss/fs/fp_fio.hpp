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
 * @file fp_fio.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-18
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "file_def.hpp"
#include "arss/ds/buf.hpp"

namespace arss {

namespace fs {

file_dec_t fpfio_open(const char* filepath, const char* mode);
bool fpfio_valid(file_dec_t& fp);
void fpfio_close(file_dec_t &fp);
int fpfio_seek(file_dec_t &fp, off_t off, int whence);
void fpfio_sync(file_dec_t &fp);
int64_t fpfio_read(file_dec_t &fp, void* ptr, size_t len);
int64_t fpfio_write(file_dec_t &fp, const void* ptr, size_t len);
int64_t fpfio_readall1(file_dec_t &fp, ds::Buf& buf);
int64_t fpfio_readall2(file_dec_t &fp, std::string& buf);
int64_t fpfio_readall3(file_dec_t &fp, void* buf, size_t len);
bool fpfio_readline(file_dec_t &fp, std::string& str);
int64_t fpfio_readrange(file_dec_t& fp, std::string& str, size_t from, size_t to);

} // namespace fs

} // namespace arss
