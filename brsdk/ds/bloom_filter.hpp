/**
 * MIT License
 * 
 * Copyright ? 2022 <Jerry.Yu>.
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
 * @file bloom_filter.hpp
 * @brief 小顶堆
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
 * @brief 布隆过滤器hash接口类型
 * @param key 键值
 * @param len key的长度
 * @return uint64_t hash值
 */
typedef uint64_t (*bl_hash_fn_t)(const void *key, size_t len);

/**
 * @brief 布隆过滤器类型
 */
struct bloom_t;

/**
 * @brief 创建布隆过滤器
 * @param size 最大数据量，比如你可能存放1千万个数据就填一千万
 * @param nfuncs hash函数个数，多个散列接口能加强防碰撞能力，但是会增加耗时(需要考量提供的散列接口优良度) @ref jtu_bi_hash_fn_t 去定义接口，提供两个能用的 @ref jtu_bl_sax_hash @ref jtu_bl_sdbm_hash
 * @param ... hash函数接口，需要和nfuncs数量匹配
 * @return struct bloom_t* 布隆过滤器句柄，非空为正常
 */
struct bloom_t *bloom_creat(size_t size, size_t nfuncs, ...);

/**
 * @brief 删除布隆过滤器
 * @param bloom 句柄
 * @return int 小于0为异常，0为正常
 */
int bloom_destroy(struct bloom_t **bloom);

/**
 * @brief 添加一个数据记录到布隆过滤器
 * @param bloom 句柄
 * @param key 键值
 * @param len 键值长度
 * @return int 小于0为异常，0为正常
 */
int bloom_add(struct bloom_t *bloom, const void *key, size_t len);

/**
 * @brief 检查数据是否被记录到布隆过滤器
 * @param bloom 句柄
 * @param key 键值
 * @param len 键值长度
 * @return int -1异常，0未记录，1记录
 */
int bloom_check(struct bloom_t *bloom, const void *key, size_t len);

/**
 * @brief 散列计算函数1
 * @param key 键值
 * @param len 键值长度
 * @return uint64_t hash值
 */
uint64_t bl_sax_hash(const void *key, size_t len);

/**
 * @brief 散列计算函数2
 * @param key 键值
 * @param len 键值长度
 * @return uint64_t hash值
 */
uint64_t bl_sdbm_hash(const void *key, size_t len);

} // !namespace ds
} // !namespace brsdk
