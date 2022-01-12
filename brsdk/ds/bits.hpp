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
 * @file bits.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-18
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
 * @brief 位数据
 *
 */
struct bits_t {
    uint8_t* data;  ///< 数据
    size_t size;    ///< 数据大小
    size_t bits;    ///< offset bit
    int error;      ///< 错误码
};

// 读取指定位数的数，并转换为结果类型
#define bits_read_uint8(bits, n) (uint8_t) brsdk::ds::bits_read_n(bits, n)
#define bits_read_uint16(bits, n) (uint16_t) brsdk::ds::bits_read_n(bits, n)
#define bits_read_uint32(bits, n) (uint32_t) brsdk::ds::bits_read_n(bits, n)
#define bits_read_uint64(bits, n) (uint64_t) brsdk::ds::bits_read_n(bits, n)

// 写入指定位数的数据
#define bits_write_uint8(bits, v, n) brsdk::ds::bits_write_n(bits, (uint64_t)v, n)
#define bits_write_uint16(bits, v, n) brsdk::ds::bits_write_n(bits, (uint64_t)v, n)
#define bits_write_uint32(bits, v, n) brsdk::ds::bits_write_n(bits, (uint64_t)v, n)
#define bits_write_uint64(bits, v, n) brsdk::ds::bits_write_n(bits, (uint64_t)v, n)

/**
 * @brief 初始化位句柄
 * 
 * @param bits 句柄
 * @param data 数据
 * @param size 数据长度
 */
void bits_init(struct bits_t* bits, const void* data, size_t size);

/**
 * @brief 读取下一位的值，读取后不会做位偏移，即下次读取时仍然是该位置
 * 
 * @param bits 句柄
 * @return int -1-error, 1-value, 0-value
 */
int bits_next(struct bits_t* bits);

/**
 * @brief 读取指定位数(<=64)的值，读取后不会做位偏移，即下次读取时仍然是该位置
 * 
 * @param bits 句柄
 * @return int -1-error
 */
uint64_t bits_next_n(struct bits_t* bits, int n);

/**
 * @brief 读取下一位的值，读取后会做位偏移，即下次读取时是该位置的下一个
 * 
 * @param bits 句柄
 * @return int -1-error, 1-value, 0-value
 */
int bits_read(struct bits_t* bits);

/**
 * @brief 读取指定位数(<=64)的值，读取后会做位偏移，即下次读取时是该位置的下一个
 * 
 * @param bits 句柄
 * @return int -1-error
 */
uint64_t bits_read_n(struct bits_t* bits, int n);

/// Exp-Golomb codes
int bits_read_ue(struct bits_t* bits);
int bits_read_se(struct bits_t* bits);
int bits_read_te(struct bits_t* bits);

/**
 * @brief 写入一个位，会偏移到下一个位置
 * 
 * @param bits 句柄
 * @param v 值
 * @return int 0-ok
 */
int bits_write(struct bits_t* bits, int v);

/**
 * @brief 写入指定长度(<=64)的值，会偏移
 * 
 * @param bits 句柄
 * @param v 值
 * @param n 位数
 * @return int 0-ok
 */
int bits_write_n(struct bits_t* bits, uint64_t v, int n);

}  // namespace ds

}  // namespace brsdk
