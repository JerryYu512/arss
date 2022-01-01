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
 * @file array.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-18
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <assert.h> // for assert
#include <stddef.h> // for NULL
#include <stdlib.h> // for malloc,realloc,free
#include <string.h> // for memset,memmove
#include "brsdk/mem/mem.hpp"

#define ARRAY_INIT_SIZE     16

// #include <vector>
// typedef std::vector<type> atype;
#define BRSDK_ARRAY_DECL(type, atype) \
struct atype {      \
    type*   ptr;    \
    size_t  size;   \
    size_t  maxsize;\
};                  \
typedef struct atype atype;\
\
static inline type* atype##_data(atype* p) {\
    if (!p) { \
        return NULL; \
    }\
    return p->ptr;\
}\
\
static inline size_t atype##_size(atype* p) {\
    if (!p) { \
        return 0; \
    }\
    return p->size;\
}\
\
static inline size_t atype##_maxsize(atype* p) {\
    if (!p) { \
        return 0; \
    }\
    return p->maxsize;\
}\
\
static inline size_t atype##_empty(atype* p) {\
    if (!p) { \
        return 1; \
    }\
    return p->size == 0;\
}\
\
static inline type* atype##_at(atype* p, size_t pos) {\
    assert(pos < p->size);\
    if (!p || pos >= p->size) {\
        return NULL; \
    }\
    return p->ptr + pos;\
}\
\
static inline type* atype##_front(atype* p) {\
    if (!p) { \
        return NULL; \
    }\
    return p->size == 0 ? NULL : p->ptr;\
}\
\
static inline type* atype##_back(atype* p) {\
    if (!p) { \
        return NULL; \
    }\
    return p->size == 0 ? NULL : p->ptr + p->size - 1;\
}\
\
static inline void atype##_init(atype* p, size_t maxsize) {\
    if (!p) { \
        return ; \
    }\
    p->size = 0;\
    p->maxsize = maxsize;\
    BRSDK_ALLOC(p->ptr, sizeof(type) * maxsize);\
}\
\
static inline void atype##_clear(atype* p) {\
    if (!p) { \
        return ; \
    }\
    p->size = 0;\
    memset(p->ptr, 0, sizeof(type) * p->maxsize);\
}\
\
static inline void atype##_cleanup(atype* p) {\
    if (!p) { \
        return ; \
    }\
    BRSDK_FREE(p->ptr);\
    p->size = p->maxsize = 0;\
}\
\
static inline void atype##_resize(atype* p, size_t maxsize) {\
    if (!p) { \
        return ; \
    }\
    if (maxsize == 0) maxsize = ARRAY_INIT_SIZE;\
    p->ptr = (type*)brsdk::brsdk_realloc(p->ptr, sizeof(type) * maxsize, sizeof(type) * p->maxsize);\
    p->maxsize = maxsize;\
}\
\
static inline void atype##_double_resize(atype* p) {\
    if (!p) { \
        return ; \
    }\
    atype##_resize(p, p->maxsize * 2);\
}\
\
static inline void atype##_push_back(atype* p, type* elem) {\
    if (!p) { \
        return ; \
    }\
    if (p->size == p->maxsize) {\
        atype##_double_resize(p);\
    }\
    p->ptr[p->size] = *elem;\
    p->size++;\
}\
\
static inline void atype##_pop_back(atype* p) {\
    assert(p->size > 0);\
    if (!p || p->size <= 0) { \
        return ; \
    }\
    p->size--;\
}\
\
static inline void atype##_add(atype* p, type* elem, size_t pos) {\
    assert(pos <= p->size);\
    if (!p || pos > p->size) { \
        return ; \
    }\
    if (p->size == p->maxsize) {\
        atype##_double_resize(p);\
    }\
    if (pos < p->size) {\
        memmove(p->ptr + pos+1, p->ptr + pos, sizeof(type) * (p->size - pos));\
    }\
    p->ptr[pos] = *elem;\
    p->size++;\
}\
\
static inline void atype##_del(atype* p, size_t pos) {\
    assert(pos < p->size);\
    if (!p || pos >= p->size) { \
        return ; \
    }\
    p->size--;\
    if (pos < p->size) {\
        memmove(p->ptr + pos, p->ptr + pos+1, sizeof(type) * (p->size - pos));\
    }\
}\
\
static inline void atype##_del_nomove(atype* p, size_t pos) {\
    assert(pos < p->size);\
    if (!p || pos >= p->size) { \
        return ; \
    }\
    p->size--;\
    if (pos < p->size) {\
        p->ptr[pos] = p->ptr[p->size];\
    }\
}\
\
static inline void atype##_swap(atype* p, size_t pos1, size_t pos2) {\
    if (!p || pos1 >= p->size || pos2 >= p->size) { \
        return ; \
    }\
    type tmp = p->ptr[pos1];\
    p->ptr[pos1] = p->ptr[pos2];\
    p->ptr[pos2] = tmp;\
}
