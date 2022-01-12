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
 * @file ssl.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-25
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <string.h>

namespace brsdk {

namespace net {

typedef void* ssl_ctx_t; ///> SSL_CTX
typedef void* ssl_t; ///> SSL

///< 状态
enum {
    SSL_OK = 0,
    SSL_WANT_READ = -2,
    SSL_WANT_WRITE = -3,
};

///< 初始化参数
typedef struct {
    const char* crt_file;
    const char* key_file;
    const char* ca_file;
    short       verify_peer;
    short       endpoint; // 0: server 1: client
} ssl_ctx_init_param_t;

/*
const char* ssl_backend() {
#ifdef WITH_OPENSSL
    return "openssl";
#elif defined(WITH_MBEDTLS)
    return "mbedtls";
#else
    return "null";
#endif
}
*/

/**
 * @brief 获取当前加密方式
 * 
 * @return const char* openssl, mbedtls, null
 */
const char* ssl_backend();

#define BRSDK_WITH_SSL (strcmp(brsdk::net::ssl_backend(), "null") != 0)

///< 初始化
ssl_ctx_t ssl_ctx_init(ssl_ctx_init_param_t* param);

///< 释放
void ssl_ctx_cleanup(ssl_ctx_t ssl_ctx);

///< 获取实例，全局的
ssl_ctx_t ssl_ctx_instance();

///< 创建ssl连接
ssl_t ssl_new(ssl_ctx_t ssl_ctx, int fd);

///< 释放
void ssl_free(ssl_t ssl);

///< 网络操作
int ssl_accept(ssl_t ssl);
int ssl_connect(ssl_t ssl);
int ssl_read(ssl_t ssl, void* buf, int len);
int ssl_write(ssl_t ssl, const void* buf, int len);
int ssl_close(ssl_t ssl);

} // namespace net

} // namespace brsdk
