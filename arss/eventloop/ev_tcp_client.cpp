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
 * @file ev_tcp_client.cpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-19
 *
 * @copyright MIT License
 *
 */
#define CUSTOM_MODULE_NAME "net"
#include "ev_tcp_client.hpp"
#include "arss/log/logging.hpp"
#include "ev_connector.hpp"
#include "evloop.hpp"

#include <stdio.h>

namespace arss {

namespace net {

namespace detail {

void removeConnection(EventLoop* loop, const EvTcpConnectionPtr& conn) {
    loop->queueInLoop(std::bind(&EvTcpConnection::connectDestroyed, conn));
}

void removeConnector(const EvConnectorPtr& connector) {
    // connector->
}

}  // namespace detail

EvTcpClient::EvTcpClient(EventLoop* loop, const EvInetAddress& serverAddr, const std::string& nameArg)
    : loop_(CHECK_NOTNULL(loop)),
      connector_(new EvConnector(loop, serverAddr)),
      name_(nameArg),
      connectionCallback_(ev_default_connection_callback),
      messageCallback_(ev_default_message_callback),
      retry_(false),
      connect_(true),
      nextConnId_(1) {
    connector_->setNewConnectionCallback(std::bind(&EvTcpClient::newConnection, this, _1));
    // FIXME setConnectFailedCallback
    LOG_INFO << "EvTcpClient::EvTcpClient[" << name_ << "] - connector " << get_pointer(connector_);
}

EvTcpClient::~EvTcpClient() {
    LOG_INFO << "EvTcpClient::~EvTcpClient[" << name_ << "] - connector " << get_pointer(connector_);
    EvTcpConnectionPtr conn;
    bool unique = false;
    {
        MutexLockGuard lock(mutex_);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn) {
        assert(loop_ == conn->getLoop());
        // FIXME: not 100% safe, if we are in different thread
        EvCloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
        loop_->runInLoop(std::bind(&EvTcpConnection::setCloseCallback, conn, cb));
        if (unique) {
            conn->forceClose();
        }
    } else {
        connector_->stop();
        // FIXME: HACK
        loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    }
}

void EvTcpClient::connect() {
    // FIXME: check state
    LOG_INFO << "EvTcpClient::connect[" << name_ << "] - connecting to "
             << connector_->serverAddress().to_ipport();
    connect_ = true;
    connector_->start();
}

void EvTcpClient::disconnect() {
    connect_ = false;

    {
        MutexLockGuard lock(mutex_);
        if (connection_) {
            connection_->shutdown();
        }
    }
}

void EvTcpClient::stop() {
    connect_ = false;
    connector_->stop();
}

void EvTcpClient::newConnection(int sockfd) {
    loop_->assertInLoopThread();
	sock_addr_t addr;
	sock_get_peer_name(sockfd, addr);
    EvInetAddress peerAddr(addr);
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peerAddr.to_ipport().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

	sock_get_name(sockfd, addr);
    EvInetAddress localAddr(addr);
    // FIXME poll with zero timeout to double confirm the new connection
    // FIXME use make_shared if necessary
    EvTcpConnectionPtr conn(new EvTcpConnection(loop_, connName, sockfd, localAddr, peerAddr));

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&EvTcpClient::removeConnection, this, _1));  // FIXME: unsafe
    {
        MutexLockGuard lock(mutex_);
        connection_ = conn;
    }
    conn->connectEstablished();
}

void EvTcpClient::removeConnection(const EvTcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    assert(loop_ == conn->getLoop());

    {
        MutexLockGuard lock(mutex_);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queueInLoop(std::bind(&EvTcpConnection::connectDestroyed, conn));
    if (retry_ && connect_) {
        LOG_INFO << "EvTcpClient::connect[" << name_ << "] - Reconnecting to "
                 << connector_->serverAddress().to_ipport();
        connector_->restart();
    }
}

}  // namespace net

}  // namespace arss
