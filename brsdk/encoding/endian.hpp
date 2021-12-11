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
 * @file endian.hpp
 * @brief 大小端
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "brsdk/defs/defs.hpp"
#include <stdint.h>
#include <string.h>

namespace brsdk {

/**
 * @brief 大小端检测
 * 
 * @return int @ref BRSDK_BIG_ENDIAN-大端， @ref BRSDK_LITTLE_ENDIAN小端
 */
static inline int detect_endian(void) {
    union {
        char c;
        short s;
    } u;
    u.s = 0x1122;
    if (u.c == 0x11) {
        return BRSDK_BIG_ENDIAN;
    }
    return BRSDK_LITTLE_ENDIAN;
}

/**
 * @brief 序列化
 * 
 * @tparam T 需要转换的数值类型
 * @param buf 输出
 * @param value 需要转换的数值
 * @param host_endian 主机字节序
 * @param buf_endian 需要转换到的字节序
 * @return uint8_t* buf存放结果的结束位置指针
 */
template <typename T>
uint8_t* serialize(uint8_t* buf, T value, int host_endian = LITTLE_ENDIAN, int buf_endian = BIG_ENDIAN) {
    size_t size = sizeof(T);
    uint8_t* pDst = buf;
    uint8_t* pSrc = (uint8_t*)&value;

    if (host_endian == buf_endian) {
        memcpy(pDst, pSrc, size);
    }
    else {
        for (int i = 0; i < size; ++i) {
            pDst[i] = pSrc[size-i-1];
        }
    }

    return buf+size;
}

/**
 * @brief 反序列化
 * 
 * @tparam T 需要转换的数值类型
 * @param buf 输入buf
 * @param value 转换后的值指针
 * @param host_endian 主机字节序
 * @param buf_endian 需要被转换字节序
 * @return uint8_t* 转换了的buf截止位置
 */
template <typename T>
uint8_t* deserialize(uint8_t* buf, T* value, int host_endian = LITTLE_ENDIAN, int buf_endian = BIG_ENDIAN) {
    size_t size = sizeof(T);
    uint8_t* pSrc = buf;
    uint8_t* pDst = (uint8_t*)value;

    if (host_endian == buf_endian) {
        memcpy(pDst, pSrc, size);
    }
    else {
        for (int i = 0; i < size; ++i) {
            pDst[i] = pSrc[size-i-1];
        }
    }

    return buf+size;
}

} // namespace brsdk
