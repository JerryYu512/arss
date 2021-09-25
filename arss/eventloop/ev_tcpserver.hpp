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
 * @file ev_tcpserver.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once

#include "arss/mix/types.hpp"
#include "ev_tcp_connection.hpp"

#include <atomic>
#include <map>

namespace arss {

namespace net {

class EvAcceptor;
class EventLoop;
class EventLoopThreadPool;

///
/// TCP server, supports single-threaded and thread-pool models.
///
/// This is an interface class, so don't expose too much details.
class EvTcpServer : noncopyable {
public:
    using EvThreadInitCallback = std::function<void(EventLoop*)>;
    enum Option {
        kNoReusePort,
        kReusePort,
    };

    // EvTcpServer(EventLoop* loop, const EvInetAddress& listenAddr);
    EvTcpServer(EventLoop* loop, const EvInetAddress& listenAddr, const std::string& nameArg,
              Option option = kNoReusePort);
    ~EvTcpServer();  // force out-line dtor, for std::unique_ptr members.

    const std::string& ipPort() const { return ipPort_; }
    const std::string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }

    /// Set the number of threads for handling input.
    ///
    /// Always accepts new connection in loop's thread.
    /// Must be called before @c start
    /// @param numThreads
    /// - 0 means all I/O in loop's thread, no thread will created.
    ///   this is the default value.
    /// - 1 means all I/O in another thread.
    /// - N means a thread pool with N threads, new connections
    ///   are assigned on a round-robin basis.
    void setThreadNum(int numThreads);
    void setThreadInitCallback(const EvThreadInitCallback& cb) { threadInitCallback_ = cb; }
    /// valid after calling start()
    std::shared_ptr<EventLoopThreadPool> threadPool() { return threadPool_; }

    /// Starts the server if it's not listening.
    ///
    /// It's harmless to call it multiple times.
    /// Thread safe.
    void start();

    /// Set connection callback.
    /// Not thread safe.
    void setConnectionCallback(const EvConnectionCallback& cb) { connectionCallback_ = cb; }

    /// Set message callback.
    /// Not thread safe.
    void setMessageCallback(const EvMessageCallback& cb) { messageCallback_ = cb; }

    /// Set write complete callback.
    /// Not thread safe.
    void setWriteCompleteCallback(const EvWriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

private:
    /// Not thread safe, but in loop
    void newConnection(int sockfd, const EvInetAddress& peerAddr);
    /// Thread safe.
    void removeConnection(const EvTcpConnectionPtr& conn);
    /// Not thread safe, but in loop
    void removeConnectionInLoop(const EvTcpConnectionPtr& conn);

    typedef std::map<std::string, EvTcpConnectionPtr> ConnectionMap;

    EventLoop* loop_;  // the acceptor loop
    const std::string ipPort_;
    const std::string name_;
    std::unique_ptr<EvAcceptor> acceptor_;  // avoid revealing Acceptor
    std::shared_ptr<EventLoopThreadPool> threadPool_;
    EvConnectionCallback connectionCallback_;
    EvMessageCallback messageCallback_;
    EvWriteCompleteCallback writeCompleteCallback_;
    EvThreadInitCallback threadInitCallback_;
    std::atomic_int32_t started_;
    // always in loop thread
    int nextConnId_;
    ConnectionMap connections_;
};

}  // namespace net

}  // namespace arss
