/**
 * MIT License
 *
 * Copyright © 2021 <Jerry.Yu>.
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
 * @file ev_type.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-06
 *
 * @copyright MIT License
 *
 */
#pragma once

#include "arss/time/timestamp.hpp"

#include <functional>
#include <memory>

namespace arss {

namespace net {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

// should really belong to base/Types.h, but <memory> is not included there.

template <typename T>
inline T* get_pointer(const std::shared_ptr<T>& ptr) {
    return ptr.get();
}

template <typename T>
inline T* get_pointer(const std::unique_ptr<T>& ptr) {
    return ptr.get();
}

// Adapted from google-protobuf stubs/common.h
// see License in muduo/base/Types.h
template <typename To, typename From>
inline ::std::shared_ptr<To> down_pointer_cast(const ::std::shared_ptr<From>& f) {
    if (false) {
        implicit_cast<From*, To*>(0);
    }

#ifndef NDEBUG
    assert(f == NULL || dynamic_cast<To*>(get_pointer(f)) != NULL);
#endif
    return ::std::static_pointer_cast<To>(f);
}

// All client visible callbacks go here.

class EvBuffer;
class EvTcpConnection;
class EventLoop;
using  EvTcpConnectionPtr = std::shared_ptr<EvTcpConnection>;
using EvTimerCallback = std::function<void()>;
using EvConnectionCallback = std::function<void(const EvTcpConnectionPtr&)>;
using EvCloseCallback = std::function<void(const EvTcpConnectionPtr&)>;
using EvWriteCompleteCallback = std::function<void(const EvTcpConnectionPtr&)>;
using EvHighWaterMarkCallback = std::function<void(const EvTcpConnectionPtr&, size_t)>;

// the data has been read to (buf, len)
using EvMessageCallback = std::function<void(const EvTcpConnectionPtr&, EvBuffer*, Timestamp)>;

void ev_default_connection_callback(const EvTcpConnectionPtr& conn);
void ev_default_message_callback(const EvTcpConnectionPtr& conn, EvBuffer* buffer,
                                 Timestamp receiveTime);

}  // namespace net

}  // namespace arss
