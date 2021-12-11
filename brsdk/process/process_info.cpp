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
 * @file process_info.cpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#include "process_info.hpp"
#include "brsdk/thread/current_thread.hpp"
#include "brsdk/fs/file.hpp"

#include <algorithm>
#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>  // snprintf
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <unistd.h>

namespace brsdk {

namespace ProcessInfo {

namespace detail {
__thread int t_numOpenedFiles = 0;
int fdDirFilter(const struct dirent* d) {
    if (::isdigit(d->d_name[0])) {
        ++t_numOpenedFiles;
    }
    return 0;
}

__thread std::vector<pid_t>* t_pids = NULL;
int taskDirFilter(const struct dirent* d) {
    if (::isdigit(d->d_name[0])) {
        t_pids->push_back(atoi(d->d_name));
    }
    return 0;
}

int scanDir(const char* dirpath, int (*filter)(const struct dirent*)) {
    struct dirent** namelist = NULL;
    int result = ::scandir(dirpath, &namelist, filter, alphasort);
    assert(namelist == NULL);
    return result;
}

Timestamp g_startTime = Timestamp::now();
// assume those won't change during the life time of a process.
int g_clockTicks = static_cast<int>(::sysconf(_SC_CLK_TCK));
int g_pageSize = static_cast<int>(::sysconf(_SC_PAGE_SIZE));
}  // namespace detail

using namespace detail;

pid_t pid() { return ::getpid(); }

std::string pidString() {
    char buf[32];
    snprintf(buf, sizeof buf, "%d", pid());
    return buf;
}

uid_t uid() { return ::getuid(); }

std::string username() {
    struct passwd pwd;
    struct passwd* result = NULL;
    char buf[8192];
    const char* name = "unknownuser";

    getpwuid_r(uid(), &pwd, buf, sizeof buf, &result);
    if (result) {
        name = pwd.pw_name;
    }
    return name;
}

uid_t euid() { return ::geteuid(); }

Timestamp startTime() { return g_startTime; }

int clockTicksPerSecond() { return g_clockTicks; }

int pageSize() { return g_pageSize; }

std::string hostname() {
    // HOST_NAME_MAX 64
    // _POSIX_HOST_NAME_MAX 255
    char buf[256];
    if (::gethostname(buf, sizeof buf) == 0) {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    } else {
        return "unknownhost";
    }
}

std::string procname() { return procname(procStat()).as_string(); }

brsdk::str::StringPiece procname(const std::string& stat) {
    brsdk::str::StringPiece name;
    size_t lp = stat.find('(');
    size_t rp = stat.rfind(')');
    if (lp != std::string::npos && rp != std::string::npos && lp < rp) {
        name.set(stat.data() + lp + 1, static_cast<int>(rp - lp - 1));
    }
    return name;
}

std::string procStatus() {
    std::string result;
    fs::readFile("/proc/self/status", 65536, &result);
    return result;
}

std::string procStat() {
    std::string result;
    fs::readFile("/proc/self/stat", 65536, &result);
    return result;
}

std::string threadStat() {
    char buf[64];
    snprintf(buf, sizeof buf, "/proc/self/task/%d/stat", thread::tid());
    std::string result;
    fs::readFile(buf, 65536, &result);
    return result;
}

std::string exePath() {
    std::string result;
    char buf[1024];
    ssize_t n = ::readlink("/proc/self/exe", buf, sizeof buf);
    if (n > 0) {
        result.assign(buf, n);
    }
    return result;
}

int openedFiles() {
    t_numOpenedFiles = 0;
    scanDir("/proc/self/fd", fdDirFilter);
    return t_numOpenedFiles;
}

int maxOpenFiles() {
    struct rlimit rl;
    if (::getrlimit(RLIMIT_NOFILE, &rl)) {
        return openedFiles();
    } else {
        return static_cast<int>(rl.rlim_cur);
    }
}

CpuTime cpuTime() {
    CpuTime t;
    struct tms tms;
    if (::times(&tms) >= 0) {
        const double hz = static_cast<double>(clockTicksPerSecond());
        t.userSeconds = static_cast<double>(tms.tms_utime) / hz;
        t.systemSeconds = static_cast<double>(tms.tms_stime) / hz;
    }
    return t;
}

int numThreads() {
    int result = 0;
    std::string status = procStatus();
    size_t pos = status.find("Threads:");
    if (pos != std::string::npos) {
        result = ::atoi(status.c_str() + pos + 8);
    }
    return result;
}

std::vector<pid_t> threads() {
    std::vector<pid_t> result;
    t_pids = &result;
    scanDir("/proc/self/task", taskDirFilter);
    t_pids = NULL;
    std::sort(result.begin(), result.end());
    return result;
}

}  // namespace ProcessInfo

}  // namespace brsdk
