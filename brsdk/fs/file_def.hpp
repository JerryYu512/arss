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
 * @file file_def.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-17
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include <string>

namespace brsdk {

namespace fs {

#define BRSDK_MAX_PATH 256

/// 目录对象
typedef struct dir_s {
    char    name[BRSDK_MAX_PATH];
    char    type; // f:file d:dir l:link b:block c:char s:socket p:pipe
    char    reserverd;
    unsigned short mode;
    size_t  size;
    time_t  atime;
    time_t  mtime;
    time_t  ctime;
} dir_t;

/// 文件类型
typedef enum file_type {
    F_NORMAL,
    F_DIR,
    F_LINK,
    F_SOCKET,
    F_DEVICE,
} file_type_t;

/// 文件信息
typedef struct file_info {
    uint64_t modify_sec;
    uint64_t access_sec;
    enum file_type type;
    char path[BRSDK_MAX_PATH];
    uint64_t size;
} file_info_t;

/// 文件状态
typedef struct file_systat {
    uint64_t size_total;
    uint64_t size_avail;
    uint64_t size_free;
    char fs_type_name[32];
} file_systat;

typedef enum file_backend_type {
    FILE_BACKEND_IO,
    FILE_BACKEND_FIO,
    FILE_BACKEND_NUM,
} file_backend_type;

/*
 * Most of these MAGIC constants are defined in /usr/include/linux/magic.h,
 * and some are hardcoded in kernel sources.
 */
typedef enum fs_type_supported : int64_t {
    FS_CIFS     = 0xFF534D42,
    FS_CRAMFS   = 0x28cd3d45,
    FS_DEBUGFS  = 0x64626720,
    FS_DEVFS    = 0x1373,
    FS_DEVPTS   = 0x1cd1,
    FS_EXT      = 0x137D,
    FS_EXT2_OLD = 0xEF51,
    FS_EXT2     = 0xEF53,
    FS_EXT3     = 0xEF53,
    FS_EXT4     = 0xEF53,
    FS_FUSE     = 0x65735546,
    FS_JFFS2    = 0x72b6,
    FS_MQUEUE   = 0x19800202,
    FS_MSDOS    = 0x4d44,
    FS_NFS      = 0x6969,
    FS_NTFS     = 0x5346544e,
    FS_PROC     = 0x9fa0,
    FS_RAMFS    = 0x858458f6,
    FS_ROMFS    = 0x7275,
    FS_SELINUX  = 0xf97cff8c,
    FS_SMB      = 0x517B,
    FS_SOCKFS   = 0x534F434B,
    FS_SQUASHFS = 0x73717368,
    FS_SYSFS    = 0x62656572,
    FS_TMPFS    = 0x01021994
} fs_type_supported_t;

typedef struct {
    union {
        FILE *fp;
        int fd;
    } fd;
    file_backend_type type;
    std::string name;
} file_dec_t;

} // namespace fs

} // namespace brsdk
