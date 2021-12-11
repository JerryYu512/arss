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
 * @file ev_typedef.hpp
 * @brief 事件驱动类型定义
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-29
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include "brsdk/time/timestamp.hpp"
#include "brsdk/net/buffer.hpp"
#include "brsdk/net/socket/address.hpp"

namespace brsdk {

namespace net {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

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

// 事件循环，将驱动所有事件处理
class EventLoop;
class EventLoopThread;
class EventLoopThreadPool;

// 事件轮询器
// 管理事件通道：添加，更新，删除
// 轮询事件，产生时间通道
class EventPoller;

// 事件通道
// 1.IO请求通过事件通道注册到事件轮询器；
// 2.事件轮询器产生IO事件发生的事件通道；
// 3.事件循环处理事件通道；
// 4.事件通道通知IO可以操作；
// 5.IO操作；
class EventChannel;
using EventChanneList = std::vector<EventChannel*>;
using EventChannelMap = std::map<int, EventChannel*>;

// 网络连接，代表一个已经建立的连接
// 实际IO在连接中处理
// 事件通道创建者
class Connection;
class TcpConnection;
class UdpConnection;

// 网络连接器
// 用于发起网络连接请求，处理请求结果
// 适用于TCP/UDP客户端
class Connector;
// class TcpConnector;
// class UdpConnector;

// 网络接收器
// 用于处理网络连接请求
class Acceptor;

// 网络数据缓冲区，用于reactor模式使用
using Buffer = NetBuffer;

// 事件循环线程池，将事件通道处理分散到多线程内处理，主线程仅仅查询轮询器
class EventLoopThreadPool;

// 事件循环协程实现
class EventLoopCoroutine;

// tcp连接器指针
using TcpConnectorPtr = std::shared_ptr<Connector>;
// tcp连接指针
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

// callback
// 线程初始化回调接口
using ThreadInitCallback = std::function<void(EventLoop*)>;
// 事件回调
using EventCallback = std::function<void()>;
// loop处理接口
using EventFunctor = std::function<void()>;
using ReadEventCallback = std::function<void(Timestamp)>;
// 新连接回调
using AcceptNewConnectionCallback = std::function<void(int sockfd, const Address& addr)>;
using ConnectorNewConnectionCallback = std::function<void(int sockfd)>;
// 定时器超时
using TimerCallback = std::function<void()>;
// tcp连接成功
using TcpConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
// tcp关闭
using TcpCloseCallbak = std::function<void(const TcpConnectionPtr&)>;
// tcp消息发送成功
using TcpWriteCompleteCallbak = std::function<void(const TcpConnectionPtr&)>;
//
using TcpHighWaterMarkCallbak = std::function<void(const TcpConnectionPtr&, size_t)>;
// tcp消息接收成功
using TcpMessageCallbak = std::function<void(const TcpConnectionPtr&, NetBuffer*, Timestamp)>;

class TcpClient;
class TcpServer;
class UdpClient;
class UdpServer;
class File;
class Timer;

} // namespace net

} // namespace brsdk
