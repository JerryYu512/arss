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
 * @file mem.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "mem.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <atomic>

namespace brsdk {

static std::atomic_long s_alloc_cnt(0);
static std::atomic_long s_free_cnt(0);

long alloc_cnt(void) {
    return s_alloc_cnt;
}

long free_cnt(void) {
    return s_free_cnt;
}

void memroy_check(void) {
    printf("Memcheck => alloc:%ld free:%ld\n", alloc_cnt(), free_cnt());
}

void *brsdk_malloc(size_t size) {
    s_alloc_cnt++;
    void *ptr = ::malloc(size);
    if (!ptr) {
        fprintf(stderr, "malloc failed!\n");
        return nullptr;
    }
    return ptr;
}

int brsdk_memalign(void **ptr, size_t alignment, size_t size) {
    s_alloc_cnt++;
    int ret = ::posix_memalign(ptr, alignment, size);
    if (ret != 0) {
        fprintf(stderr, "memalign failed!\n");
    }

    return ret;
}

void *brsdk_realloc(void *oldptr, size_t newsize, size_t oldsize) {
    s_alloc_cnt++;
    s_free_cnt++;
    void* ptr = ::realloc(oldptr, newsize);
    if (!ptr) {
        fprintf(stderr, "realloc failed!\n");
        return nullptr;
    }
    if (newsize > oldsize) {
        memset((char*)ptr + oldsize, 0, newsize - oldsize);
    }
    return ptr;
}

void *brsdk_calloc(size_t nmemb, size_t size) {
    s_alloc_cnt++;
    void* ptr =  ::calloc(nmemb, size);
    if (!ptr) {
        fprintf(stderr, "calloc failed!\n");
        return nullptr;
    }
    return ptr;
}

void *brsdk_zalloc(size_t size) {
    s_alloc_cnt++;
    void* ptr = ::malloc(size);
    if (!ptr) {
        fprintf(stderr, "malloc failed!\n");
        return nullptr;
    }
    memset(ptr, 0, size);
    return ptr;
}

void brsdk_free(void *ptr) {
    if (ptr) {
        ::free(ptr);
        ptr = NULL;
        s_free_cnt++;
    }
}

void memzero(void *ptr, size_t len) {
    memset(ptr, 0, len);
}

} // namespace brsdk
