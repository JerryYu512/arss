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

namespace arss {

/**
 * @brief 内存分配接口
 */
typedef struct {
    void *(*malloc)(size_t);
    int (*memalign)(void **, size_t, size_t);
    void *(*realloc)(void *, size_t);
    void *(*calloc)(size_t, size_t);
    void (*free)(void *);
} memory_conf_t;

/**
 * @brief 内存函数初始化
 * @param conf 配置
 */
void arss_memory_init(const memory_conf_t &conf);

/**
 * @brief 内存分配
 * @param size 内存大小
 * @return 内存地址
 */
void *arss_malloc(size_t size);

/**
 * @brief
 * @param ptr
 * @param alignment
 * @param size
 * @return
 */
int arss_memalign(void **ptr, size_t alignment, size_t size);

///< 内存重新分配
void *arss_realloc(void *oldptr, size_t newsize, size_t oldsize);

///< 内存分配
void *arss_calloc(size_t nmemb, size_t size);

///< 内存分配后清零
void *arss_zalloc(size_t size);

///< 内存释放
void arss_free(void *ptr);

///< 内存清零
void memzero(void *ptr, size_t len);

///< 申请次数
long alloc_cnt(void);

///< 释放次数
long free_cnt(void);

///< 内存检查
void memroy_check(void);

static inline void memcheck_register(void) { atexit(memroy_check); }

#define ARSS_ALLOC(ptr, size)                       \
    do {                                            \
        *(void **)&(ptr) = arss::arss_zalloc(size); \
    } while (0)

#define ARSS_ALLOC_SIZEOF(ptr) ARSS_ALLOC(ptr, sizeof(*(ptr)))

#define ARSS_FREE(ptr)            \
    do {                          \
        if (ptr) {                \
            arss::arss_free(ptr); \
            ptr = NULL;           \
        }                         \
    } while (0)

}  // namespace arss
