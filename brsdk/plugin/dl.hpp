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
 * @file dl.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-25
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <dlfcn.h>

namespace brsdk {

/**
 * @brief 加载模式
 * 
 */
typedef enum : int {
    DL_LOCAL = RTLD_LOCAL,
    DL_LAZY = RTLD_LAZY,        ///< 懒加载，需要用的符号再去找
    DL_NOW = RTLD_NOW,          ///< 立即找到所有未解决的符号
    DL_GLOBAL = RTLD_GLOBAL,    ///< 全局，当后加载的库需要用到的符号在当前的这个库里的时候可以用它里边的
} DL_MODE;

/**
 * @brief 打开动态库
 * 
 * @param path 路径
 * @param mode 模式
 * @return void* 库句柄
 */
static inline void *dlopen(const char *path, int mode) {
    return ::dlopen(path, mode);
}

/**
 * @brief 关闭动态库
 * 
 * @param handle 句柄
 * @return int <0异常
 */
static inline int dlclose(void *handle) {
    return ::dlclose(handle);
}

/**
 * @brief 查找符号
 * 
 * @param handle 句柄
 * @param symbol 符号名称
 * @return void* 符号地址，自行强转为需要的类型
 */
static inline void *dlsym(void *handle, const char *symbol) {
    return ::dlsym(handle, symbol);
}

/**
 * @brief 错误信息
 * 
 * @return const char* 错误字符串
 */
static inline const char *dlerror(void) {
    return ::dlerror();
}

} // namespace brsdk
