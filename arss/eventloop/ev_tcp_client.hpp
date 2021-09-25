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
 * @file ev_tcp_client.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once

#include "arss/lock/mutex.hpp"
#include "ev_tcp_connection.hpp"

namespace arss {

namespace net {

class EvConnector;
using EvConnectorPtr = std::shared_ptr<EvConnector>;

class EvTcpClient : noncopyable {
public:
    // EvTcpClient(EventLoop* loop);
    // EvTcpClient(EventLoop* loop, const std::string& host, uint16_t port);
    EvTcpClient(EventLoop* loop, const EvInetAddress& serverAddr, const std::string& nameArg);
    ~EvTcpClient();  // force out-line dtor, for std::unique_ptr members.

    void connect();
    void disconnect();
    void stop();

    EvTcpConnectionPtr connection() const {
        MutexLockGuard lock(mutex_);
        return connection_;
    }

    EventLoop* getLoop() const { return loop_; }
    bool retry() const { return retry_; }
    void enableRetry() { retry_ = true; }

    const std::string& name() const { return name_; }

    /// Set connection callback.
    /// Not thread safe.
    void setConnectionCallback(EvConnectionCallback cb) { connectionCallback_ = std::move(cb); }

    /// Set message callback.
    /// Not thread safe.
    void setMessageCallback(EvMessageCallback cb) { messageCallback_ = std::move(cb); }

    /// Set write complete callback.
    /// Not thread safe.
    void setWriteCompleteCallback(EvWriteCompleteCallback cb) {
        writeCompleteCallback_ = std::move(cb);
    }

private:
    /// Not thread safe, but in loop
    void newConnection(int sockfd);
    /// Not thread safe, but in loop
    void removeConnection(const EvTcpConnectionPtr& conn);

    EventLoop* loop_;
    EvConnectorPtr connector_;  // avoid revealing EvConnector
    const std::string name_;
    EvConnectionCallback connectionCallback_;
    EvMessageCallback messageCallback_;
    EvWriteCompleteCallback writeCompleteCallback_;
    bool retry_;    // atomic
    bool connect_;  // atomic
    // always in loop thread
    int nextConnId_;
    mutable MutexLock mutex_;
    EvTcpConnectionPtr connection_ GUARDED_BY(mutex_);
};

}  // namespace net

}  // namespace arss
