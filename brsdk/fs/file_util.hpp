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
 * @file file_util.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <list>
#include "file_def.hpp"

namespace brsdk {

namespace fs {

////////////////////////////////////////////////
/// 文件简单操作
///////////////////////////////////////////////

/// 文件是否存在
bool fs_exists(const char* path);

/// 文件大小
int64_t fs_size(const char* path);

/// 创建文件
bool fs_create(const char *path);

/// 直接写入文件
int64_t fs_write_to(const char *path, const void *data, size_t len, off_t offset=0);

/// 直接从文件读取
int64_t fs_read_from(const char *path, void *data, size_t len, off_t offset=0);

// rf = false  ->  rm or rmdir
// rf = true   ->  rm -rf
bool fs_remove(const char* path, bool rf=false);

/// 文件重命名
bool fs_rename(const char* from, const char* to);

// administrator privileges required on windows
bool fs_symlink(const char* dst, const char* lnk);

/// 是否是目录
bool fs_isdir(const char* path);

// modify time
int64_t fs_mtime(const char* path);

// 获取文件信息
bool fs_info(const char *path, struct file_info *info);

//////////////////////////////
/// dir
/////////////////////////////

/// 列出目录下的文件
int listdir(const char* dir, std::list<dir_t>& dirs);

/// 目录大小
int dir_size(const char *path, uint64_t *size);

/// 目录下的文件数
int num_in_dir(const char *path);

/// 创建文件夹 mkdir -p
int mkdir_p(const char* dir);

/// 删除文件夹 rm -rf
int rmdir_p(const char* dir);

/// 程序执行路径
char* executable_path(char* buf, int size);
/// 执行目录
char* executable_dir(char* buf, int size);
/// 执行文件名
char* executable_file(char* buf, int size);
/// 当前运行路径
char* run_dir(char* buf, int size);

} // namespace fs

} // namespace brsdk
