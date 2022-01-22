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
 * @file os.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-25
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <string>

namespace brsdk {

namespace os {

// 设置系统调用system函数调用接口，默认system
void set_default_system_call(int (*fun)(const char *));

/**
 * @brief 执行系统shell命令
 * 
 * @param cmd 命令
 * @return int 返回值 @ref system
 */
int syscmd(const char *cmd);

/**
 * @brief 内存信息
 * 
 */
typedef struct {
    unsigned long total; ///< 总大小，kb
    unsigned long free;  ///< 剩余大小，kb
} meminfo_t;

// 参考 : https://github.com/ThePhD/infoware

/**
 * @brief 获取运行时平台架构
 * 
 * @return const char* 架构名称
 */
const char *get_arch(void);

/**
 * @brief 获取运行时平台信息
 * 
 * @return const char* 平台
 */
const char *get_platform(void);

/**
 * @brief 获取系统名称
 * 
 * @return const char* 名称
 */
const char *get_sysname(void);

/**
 * @brief 获取处理器信息
 * 
 * @return const char* 处理器信息
 */
const char *get_processor(void);

/**
 * @brief 获取系统版本
 * 
 * @return const char* 版本
 */
const char *get_version(void);

/**
 * @brief 获取cpu核心数
 * 
 * @return int -1-异常
 */
int get_ncpu(void);

/**
 * @brief 获取内存信息
 * 
 * @param mem 内存
 * @return int <0异常
 */
int get_meminfo(meminfo_t &mem);

/**
 * @brief 获取环境变量
 * 
 * @param name 环境变量名
 * @return std::string 变量值
 */
std::string env(const char* name);

/**
 * @brief 以守护进程运行
 * 
 */
void daemon(void);

} // namespace os

} // namespace brsdk
