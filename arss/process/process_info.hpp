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
 * @file process_info.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <sys/types.h>
#include <vector>
#include "arss/mix/types.hpp"
#include "arss/str/string_piece.hpp"
#include "arss/time/timestamp.hpp"

namespace arss {

namespace ProcessInfo {

// 进程id
pid_t pid();

/// 进程id字符串
std::string pidString();

/// 用户id
uid_t uid();

/// 用户名
std::string username();

/// TODO
uid_t euid();

/// 程序启动时间
Timestamp startTime();
int clockTicksPerSecond();

/// 分页大小
int pageSize();

/// 主机名
std::string hostname();

/// 进程名
std::string procname();

/// 进程名
arss::str::StringPiece procname(const std::string& stat);

/// read /proc/self/status
std::string procStatus();

/// read /proc/self/stat
std::string procStat();

/// read /proc/self/task/tid/stat
std::string threadStat();

/// readlink /proc/self/exe
std::string exePath();

/// 打开的文件数量
int openedFiles();

/// 最大能打开的文件数量
int maxOpenFiles();

/// cpu时间
struct CpuTime {
    double userSeconds;
    double systemSeconds;

    CpuTime() : userSeconds(0.0), systemSeconds(0.0) {}

    double total() const { return userSeconds + systemSeconds; }
};

CpuTime cpuTime();

/// 线程数
int numThreads();

/// 线程号列表
std::vector<pid_t> threads();

}  // namespace ProcessInfo

}  // namespace arss
