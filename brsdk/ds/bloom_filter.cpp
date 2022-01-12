
/**
 * MIT License
 *
 * Copyright © 2022 <Jerry.Yu>.
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
 * @file bloom_filter.cpp
 * @brief 小顶堆
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2022-01-12
 *
 * @copyright MIT License
 *
 */
#define CUSTIOM_MODULE_NAME "DS"
#include "bloom_filter.hpp"
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "brsdk/defs/defs.hpp"
#include "brsdk/log/logging.hpp"

namespace brsdk {
namespace ds {

struct bloom_t {
    size_t asize;         ///< 位空间大小(位的容量，而不是内存容量)
    size_t nfuncs;        ///< 哈希函数数量
    unsigned char *a;     ///< 位阵列
    bl_hash_fn_t *funcs;  ///< 哈希函数列表
};

uint64_t bl_sax_hash(const void *key, size_t len) {
    uint64_t h = 0;
    unsigned char *ptr = (unsigned char *)key;

    while (len) {
        h ^= (h << 5) + (h >> 2) + *ptr;
        ptr++;
        len--;
    }

    return h;
}

uint64_t bl_sdbm_hash(const void *key, size_t len) {
    uint64_t h = 0;
    unsigned char *ptr = (unsigned char *)key;

    while (len) {
        h = *ptr + (h << 6) + (h << 16) - h;
        ptr++;
        len--;
    }

    return h;
}

struct bloom_t *bloom_creat(size_t size, size_t nfuncs, ...) {
    if (size <= 0 || nfuncs <= 0) {
		LOG_ERROR << "The size or hash function num is zero.";
        return NULL;
    }

    struct bloom_t *bloom;
    va_list l;

    if (!(bloom = (struct bloom_t *)malloc(sizeof(struct bloom_t)))) {
		LOG_PARAM_NULL(bloom);
        return NULL;
    }

    if (!(bloom->a = (unsigned char *)calloc((size + CHAR_BIT - 1) / CHAR_BIT, sizeof(char)))) {
		LOG_PARAM_NULL(bloom->a);
        free(bloom);
        return NULL;
    }

    if (!(bloom->funcs = (bl_hash_fn_t *)malloc(nfuncs * sizeof(bl_hash_fn_t)))) {
		LOG_PARAM_NULL(bloom->funcs);
        free(bloom->a);
        free(bloom);
        return NULL;
    }

    va_start(l, nfuncs);
    for (size_t n = 0; n < nfuncs; ++n) {
        bloom->funcs[n] = va_arg(l, bl_hash_fn_t);
        if (NULL == bloom->funcs[n]) {
			LOG_ERROR << "hash function[" << n << "] is null";
            free(bloom->a);
            free(bloom->funcs);
            free(bloom);
    		va_end(l);
            return NULL;
        }
    }
    va_end(l);

    bloom->nfuncs = nfuncs;
    bloom->asize = size;

    return bloom;
}

int bloom_destroy(struct bloom_t **bloom) {
    if (!bloom || !*bloom) {
		LOG_PARAM_NULL(bloom);
		LOG_PARAM_NULL(*bloom);
        return -1;
    }

    free((*bloom)->a);
    free((*bloom)->funcs);
    free(*bloom);

    *bloom = NULL;

    return 0;
}

int bloom_add(struct bloom_t *bloom, const void *key, size_t len) {
    if (!bloom || !key || len <= 0) {
		LOG_PARAM_NULL(bloom);
		LOG_PARAM_NULL(key);
		LOG_PARAM_OVER_RANGE(len, 0, ULONG_MAX);
        return -1;
    }

    size_t n = 0;

    for (n = 0; n < bloom->nfuncs; ++n) {
        ///< 使用多个哈希函数计算散列值并进行记录，那么检查记录时就进行多次检查，&&操作，加强防碰撞能力
        BRSDK_BITMAP_SET(bloom->a, bloom->funcs[n](key, len) % bloom->asize);
    }

    return 0;
}

int bloom_check(struct bloom_t *bloom, const void *key, size_t len) {
    if (!bloom || !key || len <= 0) {
		LOG_PARAM_NULL(bloom);
		LOG_PARAM_NULL(key);
		LOG_PARAM_OVER_RANGE(len, 0, ULONG_MAX);
        return -1;
    }

    size_t n = 0;

    for (n = 0; n < bloom->nfuncs; ++n) {
        ///< 如果有一个哈希函数表明未记录则未记录
        if (!(BRSDK_BITMAP_GET(bloom->a, bloom->funcs[n](key, len) % bloom->asize))) {
			LOG_DEBUG << "function[" << n << "] be seted.";
            return 0;
        }
    }

    return 1;
}

}  // namespace ds
}  // namespace brsdk
