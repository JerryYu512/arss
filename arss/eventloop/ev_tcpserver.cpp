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
 * @file ev_tcpserver.cpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-19
 *
 * @copyright MIT License
 *
 */
#define CUSTOM_MOUDLE_NAME

#include "ev_tcpserver.hpp"
#include "arss/log/logging.hpp"
#include "ev_acceptor.hpp"
#include "evloop.hpp"
#include "evloop_threadpool.hpp"

#include <stdio.h>

namespace arss {

namespace net {

EvTcpServer::EvTcpServer(EventLoop* loop, const EvInetAddress& listenAddr, const std::string& nameArg,
                     Option option)
    : loop_(CHECK_NOTNULL(loop)),
      ipPort_(listenAddr.to_ipport()),
      name_(nameArg),
      acceptor_(new EvAcceptor(loop, listenAddr, option == kReusePort)),
      threadPool_(new EventLoopThreadPool(loop, name_)),
      connectionCallback_(ev_default_connection_callback),
      messageCallback_(ev_default_message_callback),
      nextConnId_(1) {
    acceptor_->setNewConnectionCallback(std::bind(&EvTcpServer::newConnection, this, _1, _2));
}

EvTcpServer::~EvTcpServer() {
    loop_->assertInLoopThread();
    LOG_TRACE << "EvTcpServer::~EvTcpServer [" << name_ << "] destructing";

    for (auto& item : connections_) {
        EvTcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&EvTcpConnection::connectDestroyed, conn));
    }
}

void EvTcpServer::setThreadNum(int numThreads) {
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}

void EvTcpServer::start() {
    if (started_ == 0) {
        started_ = 1;
        threadPool_->start(threadInitCallback_);

        assert(!acceptor_->listening());
        loop_->runInLoop(std::bind(&EvAcceptor::listen, get_pointer(acceptor_)));
    }
}

void EvTcpServer::newConnection(int sockfd, const EvInetAddress& peerAddr) {
    loop_->assertInLoopThread();
    EventLoop* ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
	sock_addr_t addr;

    LOG_INFO << "EvTcpServer::newConnection [" << name_ << "] - new connection [" << connName
             << "] from " << peerAddr.to_ipport();
	sock_get_name(sockfd, addr);
    EvInetAddress localAddr(addr);
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    EvTcpConnectionPtr conn(new EvTcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&EvTcpServer::removeConnection, this, _1));  // FIXME: unsafe
    ioLoop->runInLoop(std::bind(&EvTcpConnection::connectEstablished, conn));
}

void EvTcpServer::removeConnection(const EvTcpConnectionPtr& conn) {
    // FIXME: unsafe
    loop_->runInLoop(std::bind(&EvTcpServer::removeConnectionInLoop, this, conn));
}

void EvTcpServer::removeConnectionInLoop(const EvTcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    LOG_INFO << "EvTcpServer::removeConnectionInLoop [" << name_ << "] - connection " << conn->name();
    size_t n = connections_.erase(conn->name());
    (void)n;
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&EvTcpConnection::connectDestroyed, conn));
}

}  // namespace net

}  // namespace arss
