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
 * @file mem.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <stddef.h>
#include <stdlib.h>

namespace brsdk {

/**
 * @brief 内存分配
 * @param size 内存大小
 * @return 内存地址
 */
void *brsdk_malloc(size_t size);

/**
 * @brief 内存对齐分配
 * @param ptr 指针 [out]
 * @param alignment 对齐大小
 * @param size 需要分配的大小
 * @return 0-ok，-1-error
 */
int brsdk_memalign(void **ptr, size_t alignment, size_t size);

///< 内存重新分配
void *brsdk_realloc(void *oldptr, size_t newsize, size_t oldsize);

///< 内存分配
void *brsdk_calloc(size_t nmemb, size_t size);

///< 内存分配后清零
void *brsdk_zalloc(size_t size);

///< 内存释放
void brsdk_free(void *ptr);

///< 内存清零
void memzero(void *ptr, size_t len);

///< 申请次数
long alloc_cnt(void);

///< 释放次数
long free_cnt(void);

///< 内存检查
void memroy_check(void);

///< 异常退出时检测内存
static inline void memcheck_register(void) { atexit(memroy_check); }

///< 内存分配
#define BRSDK_ALLOC(ptr, size)                        \
    do {                                              \
        *(void **)&(ptr) = brsdk::brsdk_zalloc(size); \
    } while (0)

///< 分配指针类型的内存大小
#define BRSDK_ALLOC_SIZEOF(ptr) BRSDK_ALLOC(ptr, sizeof(*(ptr)))

///< 释放
#define BRSDK_FREE(ptr)             \
    do {                            \
        if (ptr) {                  \
            brsdk::brsdk_free(ptr); \
            ptr = NULL;             \
        }                           \
    } while (0)

}  // namespace brsdk
