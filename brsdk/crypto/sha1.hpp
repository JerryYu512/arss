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
 * @file sha1.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <stdint.h>

namespace brsdk {
namespace crypto {

/**
 * @brief sha1结构
 *
 */
typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;

/**
 * @brief sha1句柄初始化
 *
 * @param context
 */
void SHA1Init(SHA1_CTX* context);

/**
 * @brief 添加数据
 * 
 * @param context 句柄
 * @param data 数据
 * @param len 数据长度
 */
void SHA1Update(SHA1_CTX* context, const unsigned char* data, uint32_t len);

/**
 * @brief 结果生成
 * 
 * @param digest 结果
 * @param context 句柄
 */
void SHA1Final(unsigned char digest[20], SHA1_CTX* context);

/**
 * @brief 计算数据的sha1
 * 
 * @param hash_out 结果
 * @param str 数据
 * @param len 数据长度
 */
void SHA1(char* hash_out, const char* str, int len);

// NOTE: if outputlen > 40: output[40] = '\0'

/**
 * @brief 计算sha1的16进制字符串
 * 
 * @param input 数据
 * @param inputlen 数据长度
 * @param output 输出
 * @param outputlen 输入缓冲区长度
 */
void sha1_hex(unsigned char* input, uint32_t inputlen, char* output, uint32_t outputlen);

}  // namespace crypto
}  // namespace brsdk
