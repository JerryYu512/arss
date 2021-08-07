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
 * @file defs.hpp
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "attr.hpp"
#include "platform.hpp"

/// 调试
#ifdef DEBUG
#define arss_printd(fmt, ...) \
    printf("[DEBUG][%s:%d:%s]" fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define arss_printe(fmt, ...) \
    fprintf("[ERROR][%s:%d:%s]" fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define arss_verbose() printf("%s:%s:%d xxxxxx\n", __FILE__, __func__, __LINE__)
#else
#define arss_printd(...)
#define arss_printe(...)
#define arss_verbose()
#endif

// TODO:增加功能注释说明
#define ARSS_DUMP_BUFFER(buf, len)                                        \
    do {                                                                  \
        int _i, _j = 0;                                                   \
        char _tmp[128] = {0};                                             \
        if (buf == NULL || len <= 0) {                                    \
            break;                                                        \
        }                                                                 \
        for (_i = 0; _i < len; _i++) {                                    \
            if (!(_i % 16)) {                                             \
                if (_i != 0) {                                            \
                    printf("%s", _tmp);                                   \
                }                                                         \
                memset(_tmp, 0, sizeof(_tmp));                            \
                _j = 0;                                                   \
                _j += snprintf(_tmp + _j, 64, "\n%p: ", buf + _i);        \
            }                                                             \
            _j += snprintf(_tmp + _j, 4, "%02hhx ", *((char *)buf + _i)); \
        }                                                                 \
        printf("%s\n", _tmp);                                             \
    } while (0)

/// 取变量的类型，c/c++是不一样的关键字
#ifndef __cplusplus
#define ARSS_TYPEOF typeof
#else
#define ARSS_TYPEOF decltype
#endif

/// 结构体成员偏移
#ifndef offsetof
#define offsetof(type, member) ((size_t)(&((type *)0)->member))
#endif

// TODO:增加注释说明
#ifndef offsetofend
#define offsetofend(type, member) (offsetof(type, member) + sizeof(((type *)0)->member))
#endif

/// 使用结构体成员地址取结构体首地址
#ifndef container_of
#define container_of(ptr, type, member)                        \
    ({                                                         \
        const __TYPEOF__(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member));     \
    })
#endif

/// 大小端
#define ARSS_BIG_ENDIAN 4321             ///< 大端
#define ARSS_LITTLE_ENDIAN 1234          ///< 小端
#define ARSS_NET_ENDIAN ARSS_BIG_ENDIAN  ///< 网络字节序

/// 字节序
#if defined(ARSS_ARCH_X86) || defined(ARSS_ARCH_X86_64)
#define ARSS_BYTE_ORDER ARSS_LITTLE_ENDIAN
#else
#error "not support"
#endif

/// 判断优化，都是判断为真的情况
#if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#else
#ifndef unlikely
#define unlikely(x) (x)
#endif
#ifndef likely
#define likely(x) (x)
#endif
#endif

/// 常用宏定义
/// 绝对值，取正值
#define ARSS_ABS(n) ((n) > 0 ? (n) : -(n))
/// 绝对值，取负值
#define ARSS_NABS(n) ((n) < 0 ? (n) : -(n))
/// 最大值
#define ARSS_MIN(a, b) ((a) > (b) ? (b) : (a))
/// 最小值
#define ARSS_MAX(a, b) ((a) > (b) ? (a) : (b))
/// 数组长度
#define ARSS_ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))
/// 字符串字面量的长度计算
#define ARSS_STRLEN(s) (sizeof(s) - 1)
/// 位置1
#define ARSS_BITSET(p, n) (*(p) |= (1u << (n)))
/// 位清0
#define ARSS_BITCLR(p, n) (*(p) &= ~(1u << (n)))
/// 取位的值
#define ARSS_BITGET(i, n) ((i) & (1u << (n)))
/// 回车
#define ARSS_CR '\r'
/// 换行
#define ARSS_LF '\n'
/// 回车换行
#define ARSS_CRLF "\r\n"
/// 浮点精度
#define ARSS_FLOAT_PRECISION 1e-6
/// 浮点值是否等于0
#define ARSS_FLOAT_EQUAL_ZERO(f) (ARSS_ABS(f) < ARSS_FLOAT_PRECISION)
/// 无限大
#define ARSS_INFINITE (uint32_t) - 1
#ifndef INFINITE
#define INFINITE (uint32_t) - 1
#endif

/*
ASCII:
[0, 0x20)    control-charaters
[0x20, 0x7F) printable-charaters

0x0A => LF
0x0D => CR
0x20 => SPACE
0x7F => DEL

[0x09, 0x0D] => \t\n\v\f\r
[0x30, 0x39] => 0~9
[0x41, 0x5A] => A~Z
[0x61, 0x7A] => a~z
*/
/// 是否是字母
#define ARSS_IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
/// 是否是数字
#define ARSS_IS_NUM(c) ((c) >= '0' && (c) <= '9')
/// 字母或数字
#define ARSS_IS_ALPHANUM(c) (ARSS_IS_ALPHA(c) || ARSS_IS_NUM(c))
// TODO:注释
#define ARSS_IS_CNTRL(c) ((c) >= 0 && (c) < 0x20)
// TODO:注释
#define ARSS_IS_GRAPH(c) ((c) >= 0x20 && (c) < 0x7F)
/// 是否是16进制字符
#define ARSS_IS_HEX(c) (ARSS_IS_NUM(c) || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
/// 是否是小写字母
#define ARSS_IS_LOWER(c) (((c) >= 'a' && (c) <= 'z'))
/// 是否是大写字母
#define ARSS_IS_UPPER(c) (((c) >= 'A' && (c) <= 'Z'))
/// 转小写
#define ARSS_LOWER(c) ((c) | 0x20)
/// 转大写
#define ARSS_UPPER(c) ((c) & ~0x20)

/// 数值对齐
#define ars_align(d, a) (((d) + (a - 1)) & ~(a - 1))
/// 指针对齐
#define ars_align_ptr(p, a) (uint8_t *)(((uintptr_t)(p) + ((uintptr_t)a - 1)) & ~((uintptr_t)a - 1))

/// 类型强转
// LD, LU, LLD, LLU for explicit conversion of integer
#define ARSS_LD(v) ((long)(v))
#define ARSS_LU(v) ((unsigned long)(v))
#define ARSS_LLD(v) ((long long)(v))
#define ARSS_LLU(v) ((unsigned long long)(v))

/// 高低4字节合成8字节
#define ARSS_MAKEINT64(h, l) (((int64_t)h) << 32 | (l & 0xffffffff))
/// 取8的高4字节
#define ARSS_HIINT(n) ((int32_t)(((int64_t)n) >> 32))
/// 取8的低4字节
#define ARSS_LOINT(n) ((int32_t)(n & 0xffffffff))
/// 合成Four-Character Codes，四字节代码，种独立标示视频数据流格式
#define ARSS_MAKE_FOURCC(a, b, c, d) \
    (((uint32)d) | (((uint32)c) << 8) | (((uint32)b) << 16) | (((uint32)a) << 24))
/// 范围限制值
#define ARSS_LIMIT(lower, v, upper) ((v) < (lower) ? (lower) : (v) > (upper) ? (upper) : (v))

/// 转为字符串
#define ARSS_STRINGIFY_HELPER(x) #x
/// 宏定义的数值也能转
#define ARSS_STRINGIFY(x) ARSS_STRINGIFY_HELPER(x)

/// 连接标识符
#define ARSS_STRINGCAT_HELPER(x, y) x##y
#define ARSS_STRINGCAT(x, y) ARSS_STRINGCAT_HELPER(x, y)

/// 安全函数操作
#define ARSS_SAFE_FREE(p) \
    do {                  \
        if (p) {          \
            free(p);      \
            (p) = NULL;   \
        }                 \
    } while (0)
#define ARSS_SAFE_DELETE(p) \
    do {                    \
        if (p) {            \
            delete (p);     \
            (p) = NULL;     \
        }                   \
    } while (0)
#define ARSS_SAFE_DELETE_ARRAY(p) \
    do {                          \
        if (p) {                  \
            delete[](p);          \
            (p) = NULL;           \
        }                         \
    } while (0)
#define ARSS_SAFE_RELEASE(p) \
    do {                     \
        if (p) {             \
            (p)->release();  \
            (p) = NULL;      \
        }                    \
    } while (0)

/// 交换值
#define ARSS_SWAP(a, b)             \
    do {                            \
        ARSS_TYPEOF(a) __tmp = (a); \
        (a) = (b);                  \
        (b) = __tmp;                \
    } while (0)

// Thread safety annotations {
// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)  // no-op
#endif

#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...) THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

// End of thread safety annotations }

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail(int errnum, const char *file, unsigned int line,
                                 const char *function) noexcept __attribute__((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret)                                                     \
    ({                                                                  \
        __typeof__(ret) errnum = (ret);                                 \
        if (__builtin_expect(errnum != 0, 0))                           \
            __assert_perror_fail(errnum, __FILE__, __LINE__, __func__); \
    })

#else  // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret)                     \
    ({                                  \
        __typeof__(ret) errnum = (ret); \
        assert(errnum == 0);            \
        (void)errnum;                   \
    })

#endif  // CHECK_PTHREAD_RETURN_VALUE
