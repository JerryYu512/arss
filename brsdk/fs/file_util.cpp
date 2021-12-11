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
 * @file file_util.cpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#include "file_util.hpp"
#include "brsdk/str/pystring.hpp"
#include "brsdk/defs/defs.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#ifdef __APPLE__
#include <sys/statvfs.h>
#else
#include <sys/statfs.h>
#include <sys/vfs.h>
#endif

namespace brsdk {

namespace fs {

static struct {
    const char name[32];
    const int64_t value;
} fs_type_info[] = {
    {"CIFS    ", FS_CIFS    },
    {"CRAMFS  ", FS_CRAMFS  },
    {"DEBUGFS ", FS_DEBUGFS },
    {"DEVFS   ", FS_DEVFS   },
    {"DEVPTS  ", FS_DEVPTS  },
    {"EXT     ", FS_EXT     },
    {"EXT2_OLD", FS_EXT2_OLD},
    {"EXT2    ", FS_EXT2    },
    {"EXT3    ", FS_EXT3    },
    {"EXT4    ", FS_EXT4    },
    {"FUSE    ", FS_FUSE    },
    {"JFFS2   ", FS_JFFS2   },
    {"MQUEUE  ", FS_MQUEUE  },
    {"MSDOS   ", FS_MSDOS   },
    {"NFS     ", FS_NFS     },
    {"NTFS    ", FS_NTFS    },
    {"PROC    ", FS_PROC    },
    {"RAMFS   ", FS_RAMFS   },
    {"ROMFS   ", FS_ROMFS   },
    {"SELINUX ", FS_SELINUX },
    {"SMB     ", FS_SMB     },
    {"SOCKFS  ", FS_SOCKFS  },
    {"SQUASHFS", FS_SQUASHFS},
    {"SYSFS   ", FS_SYSFS   },
    {"TMPFS   ", FS_TMPFS   },
};

bool fs_exists(const char* path) {
    struct stat attr;
    return ::lstat(path, &attr) == 0;
}

bool fs_isdir(const char* path) {
    struct stat attr;
    if (::lstat(path, &attr) != 0) return false;
    return S_ISDIR(attr.st_mode);
}

int64_t fs_mtime(const char* path) {
    struct stat attr;
    if (::lstat(path, &attr) != 0) return -1;
    return attr.st_mtime;
}

int64_t fs_size(const char* path) {
    struct stat attr;
    if (::lstat(path, &attr) != 0) return -1;
    return attr.st_size;
}

int fs_systat(const char *path, struct file_systat *fi) {
    uint32_t i;
    struct statfs stfs;
    if (!path || !fi) {
        // printf("path can't be null\n");
        return -1;
    }
    if (-1 == ::statfs(path, &stfs)) {
        // printf("statfs %s failed: %s\n", path, strerror(errno));
        return -1;
    }
    fi->size_total = stfs.f_bsize * stfs.f_blocks;
    fi->size_avail = stfs.f_bsize * stfs.f_bavail;
    fi->size_free  = stfs.f_bsize * stfs.f_bfree;
    for (i = 0; i < (uint32_t)BRSDK_ARRAY_SIZE(fs_type_info); i++) {
        if (stfs.f_type == fs_type_info[i].value) {
            stfs.f_type = i;
            strncpy(fi->fs_type_name, fs_type_info[i].name,
                            sizeof(fi->fs_type_name));
            break;
        }
    }
    return 0;
}

bool fs_info(const char *path, struct file_info *info) {
    struct stat st;
    if (-1 == stat(path, &st)) {
        return false;
    }
    switch (st.st_mode & S_IFMT) {
    case S_IFSOCK:
        info->type = F_SOCKET;
        break;
    case S_IFLNK:
        info->type = F_LINK;
        break;
    case S_IFBLK:
    case S_IFCHR:
        info->type = F_DEVICE;
        break;
    case S_IFREG:
        info->type = F_NORMAL;
        break;
    case S_IFDIR:
        info->type = F_DIR;
        break;
    default:
        break;
    }
    info->size = st.st_size;
#ifdef __APPLE__
    info->access_sec = st.st_atimespec.tv_sec;
    info->modify_sec = st.st_atimespec.tv_sec;//using change, not modify
#else
    info->access_sec = st.st_atim.tv_sec;
    info->modify_sec = st.st_ctim.tv_sec;//using change, not modify
#endif
    return true;
}

bool fs_create(const char* path) {
    FILE* fp = fopen(path, "w+");
    if (!fp) {
        return false;
    }
    fclose(fp);
    return true;
}

int64_t fs_write_to(const char* path, const void* data, size_t len, off_t offset) {
    FILE* fp = fopen(path, "a+");
    if (!fp) {
        return errno;
    }

    fseek(fp, offset, SEEK_SET);

    auto ret = fwrite(data, 1, len, fp);

    fclose(fp);
    return ret;
}

int64_t fs_read_from(const char *path, void *data, size_t len, off_t offset) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return errno;
    }

    fseek(fp, offset, SEEK_SET);
    auto ret = fread(data, 1, len, fp);

    fclose(fp);
    return ret;
}

// rf = false  ->  rm or rmdir
// rf = true   ->  rm -rf
bool fs_remove(const char* path, bool rf) {
    if (!fs_exists(path)) return true;

    if (!rf) {
        if (fs_isdir(path)) return ::rmdir(path) == 0;
        return ::remove(path) == 0;
    } else {
        std::string cmd("rm -rf \"");
        cmd += path;
        cmd += "\"";
        // FIXME:替换系统调用
        return system(cmd.c_str()) != -1;
    }
}

bool fs_rename(const char* from, const char* to) { return ::rename(from, to) == 0; }

bool fs_symlink(const char* dst, const char* lnk) {
    remove(lnk);
    return ::symlink(dst, lnk) == 0;
}

////////////////////////////////////
/// dir
///////////////////////////////////

static bool less(const dir_t& lhs, const dir_t& rhs) { return strcasecmp(lhs.name, rhs.name) < 0; }

int listdir(const char* dir, std::list<dir_t>& dirs) {
    int dirlen = strlen(dir);
    if (dirlen > 256) {
        return -1;
    }
    char path[512];
    strcpy(path, dir);
    if (dir[dirlen - 1] != '/') {
        strcat(path, "/");
        ++dirlen;
    }
    dirs.clear();
    // opendir -> readdir -> closedir
    DIR* dp = opendir(dir);
    if (dp == NULL) return -1;
    struct dirent* result = NULL;
    struct stat st;
    dir_t tmp;
    while ((result = readdir(dp))) {
        memset(&tmp, 0, sizeof(dir_t));
        strncpy(tmp.name, result->d_name, sizeof(tmp.name));
        strncpy(path + dirlen, result->d_name, sizeof(path) - dirlen);
        if (lstat(path, &st) == 0) {
            if (S_ISREG(st.st_mode))
                tmp.type = 'f';
            else if (S_ISDIR(st.st_mode))
                tmp.type = 'd';
            else if (S_ISLNK(st.st_mode))
                tmp.type = 'l';
            else if (S_ISBLK(st.st_mode))
                tmp.type = 'b';
            else if (S_ISCHR(st.st_mode))
                tmp.type = 'c';
            else if (S_ISSOCK(st.st_mode))
                tmp.type = 's';
            else if (S_ISFIFO(st.st_mode))
                tmp.type = 'p';
            else
                tmp.type = '-';
            tmp.mode = st.st_mode & 0777;
            tmp.size = st.st_size;
            tmp.atime = st.st_atime;
            tmp.mtime = st.st_mtime;
            tmp.ctime = st.st_ctime;
        }
        dirs.push_back(tmp);
    }
    closedir(dp);
    dirs.sort(less);
    return dirs.size();
}

static int dfs_dir_size(const char* path, uint64_t* size) {
    DIR* pdir = NULL;
    struct dirent* ent = NULL;
    char full_path[BRSDK_MAX_PATH*2];
    int ret;
    pdir = opendir(path);
    if (!pdir) {
        // printf("can not open path: %s\n", path);
        if (errno == EMFILE) {
            return -EMFILE;
        } else {
            return -1;
        }
    }
    while (NULL != (ent = readdir(pdir))) {
        memset(full_path, 0, sizeof(full_path));
        snprintf(full_path, sizeof(full_path) - 1, "%s/%s", path, ent->d_name);
        if (ent->d_type == DT_DIR) {
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
                continue;
            }
            ret = dfs_dir_size(full_path, size);
            if (ret == -EMFILE) {
                closedir(pdir);
                return ret;
            }
        } else if (ent->d_type == DT_REG) {
            *size += fs::fs_size(full_path);
        }
    }
    closedir(pdir);
    return 0;
}

int dir_size(const char* path, uint64_t* size) {
    *size = 0;
    return dfs_dir_size(path, size);
}

int num_in_dir(const char* path) {
    if (!path) {
        return -1;
    }
    DIR* dir = NULL;
    struct dirent* ent = NULL;
    int num = 0;
    dir = opendir(path);
    if (dir == NULL) {
        return -1;
    }
    while (NULL != (ent = readdir(dir))) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
            continue;
        }
        num++;
    }
    closedir(dir);
    return num;
}

int mkdir_p(const char* dir) {
    if (access(dir, 0) == 0) {
        return EEXIST;
    }
    char tmp[BRSDK_MAX_PATH] = "0";
    memcpy(tmp, dir, strlen(dir) < sizeof(tmp) ? strlen(dir) : sizeof(tmp));
    char* p = tmp;
    char delim = '/';
    while (*p) {
        if (*p == '/') {
            *p = '\0';
            mkdir(tmp, 0777);
            *p = delim;
        }
        ++p;
    }
    if (mkdir(tmp, 0777) != 0) {
        return EPERM;
    }
    return 0;
}

int rmdir_p(const char* dir) {
    if (access(dir, 0) != 0) {
        return ENOENT;
    }
    if (rmdir(dir) != 0) {
        return EPERM;
    }
    char tmp[BRSDK_MAX_PATH] = "";
    memcpy(tmp, dir, strlen(dir) < BRSDK_MAX_PATH ? strlen(dir) : BRSDK_MAX_PATH);
    char* p = tmp;
    while (*p) ++p;
    while (--p >= tmp) {
        if (*p == '/') {
            *p = '\0';
            if (rmdir(tmp) != 0) {
                return 0;
            }
        }
    }
    return 0;
}

char* executable_path(char* buf, int size) {
    if (readlink("/proc/self/exe", buf, size) == -1) {
        return NULL;
    }

    return buf;
}

char* executable_dir(char* buf, int size) {
    char filepath[BRSDK_MAX_PATH];
    executable_path(filepath, sizeof(filepath));
    std::string dir = pystring::os::path::dirname(filepath);
    snprintf(buf, size, "%s", dir.c_str());

    return buf;
}

char* executable_file(char* buf, int size) {
    char filepath[BRSDK_MAX_PATH];
    executable_path(filepath, sizeof(filepath));
    std::string f = basename(filepath);
    snprintf(buf, size, "%s", f.c_str());

    return buf;
}

char* run_dir(char* buf, int size) { return getcwd(buf, size); }

}  // namespace fs

}  // namespace brsdk
