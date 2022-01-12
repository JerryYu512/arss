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
 * @file md5.hpp
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
#include <stdint.h>

namespace brsdk {
namespace crypto {

/**
 * @brief md5数据
 * 
 */
typedef struct {
    uint64_t bytes;
    uint32_t a, b, c, d;
    uint8_t buffer[64];
} md5_t;

/**
 * @brief md5初始化句柄
 * 
 * @param ctx 
 */
void md5_init(md5_t *ctx);

/**
 * @brief 添加要计算的数据
 * 
 * @param ctx 句柄
 * @param data 数据
 * @param size 数据长度
 */
void md5_update(md5_t *ctx, const void *data, size_t size);

/**
 * @brief 计算md5结果
 * 
 * @param result 结果存放值
 * @param ctx 句柄
 */
void md5_final(uint8_t result[16], md5_t *ctx);

/**
 * @brief 直接计算摘要
 * 
 * @param data 数据
 * @param size 数据长度
 * @param result[out] 结果
 */
static inline void md5_gen(const void *data, size_t size, uint8_t result[16]) {
    md5_t md5;
    md5_init(&md5);
    md5_update(&md5, data, size);
    md5_final(result, &md5);
}

/**
 * @brief md5 16进制字符串
 * 
 * @param data 数据
 * @param size 数据长度
 * @param str[out] 结果
 */
static inline void md5_hex(const void *data, size_t size, char str[32]) {
    static const char HEX_CHARACTERS[] = "0123456789abcdef";
    uint8_t result[16] = {0};

    md5_gen(data, size, result);

    // for each byte in the hash array
    for (int i = 0; i < 16; i++) {
        // get the value of the first and second 4 bits of the byte
        uint8_t first_four_bits_value = result[i] >> 4;
        uint8_t second_four_bits_value = result[i] & 0x0f;

        // set the corresponding char for the first and second half of the byte
        str[2 * i] = HEX_CHARACTERS[first_four_bits_value];
        str[2 * i + 1] = HEX_CHARACTERS[second_four_bits_value];
    }

    // add null terminating character
    str[32] = '\0';
}

} // namespace crypto
} // namespace brsdk
 