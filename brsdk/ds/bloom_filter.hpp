/**
 * MIT License
 * 
 * Copyright ? 2022 <Jerry.Yu>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file bloom_filter.hpp
 * @brief 布隆过滤器
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2022-01-12
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace brsdk {
namespace ds {

/**
 * @brief 
 * @param key 
 * @param len 
 * @return uint64_t hashֵ
 */
typedef uint64_t (*bl_hash_fn_t)(const void *key, size_t len);

/**
 * @brief 
 */
struct bloom_t;

/**
 * @brief 
 * @param size 
 * @param nfuncs 
 * @param ... 
 * @return struct bloom_t* 
 */
struct bloom_t *bloom_creat(size_t size, size_t nfuncs, ...);

/**
 * @brief 
 * @param bloom 
 * @return int 
 */
int bloom_destroy(struct bloom_t **bloom);

/**
 * @brief 
 * @param bloom 
 * @param key 
 * @param len 
 * @return int 
 */
int bloom_add(struct bloom_t *bloom, const void *key, size_t len);

/**
 * @brief 
 * @param bloom 
 * @param key 
 * @param len 
 * @return int 
 */
int bloom_check(struct bloom_t *bloom, const void *key, size_t len);

/**
 * @brief 
 * @param key 
 * @param len 
 * @return uint64_t hash
 */
uint64_t bl_sax_hash(const void *key, size_t len);

/**
 * @brief 
 * @param key 
 * @param len 
 * @return uint64_t hashֵ
 */
uint64_t bl_sdbm_hash(const void *key, size_t len);

} // !namespace ds
} // !namespace brsdk
