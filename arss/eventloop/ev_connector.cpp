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
 * @file ev_connector.cpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-19
 *
 * @copyright MIT License
 *
 */
#define CUSTOM_MODULE_NAME "net"
#include "ev_connector.hpp"
#include <errno.h>
#include "arss/log/logging.hpp"
#include "ev_channel.hpp"
#include "evloop.hpp"

namespace arss {

namespace net {

const int EvConnector::kMaxRetryDelayMs;

EvConnector::EvConnector(EventLoop* loop, const EvInetAddress& serverAddr)
    : loop_(loop),
      serverAddr_(serverAddr),
      connect_(false),
      state_(kDisconnected),
      retryDelayMs_(kInitRetryDelayMs) {
    LOG_DEBUG << "ctor[" << this << "]";
}

EvConnector::~EvConnector() {
    LOG_DEBUG << "dtor[" << this << "]";
    assert(!channel_);
}

void EvConnector::start() {
    connect_ = true;
    loop_->runInLoop(std::bind(&EvConnector::startInLoop, this));  // FIXME: unsafe
}

void EvConnector::startInLoop() {
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if (connect_) {
        connect();
    } else {
        LOG_DEBUG << "do not connect";
    }
}

void EvConnector::stop() {
    connect_ = false;
    loop_->queueInLoop(std::bind(&EvConnector::stopInLoop, this));  // FIXME: unsafe
                                                                  // FIXME: cancel timer
}

void EvConnector::stopInLoop() {
    loop_->assertInLoopThread();
    if (state_ == kConnecting) {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void EvConnector::connect() {
    int sockfd = sock_tcp_nonblock_creat(serverAddr_.family());
    int ret = sock_connect(sockfd, *serverAddr_.get_addr());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_SYSERR << "connect error in EvConnector::startInLoop " << savedErrno;
            sock_close(sockfd);
            break;

        default:
            LOG_SYSERR << "Unexpected error in EvConnector::startInLoop " << savedErrno;
            sock_close(sockfd);
            // connectErrorCallback_();
            break;
    }
}

void EvConnector::restart() {
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void EvConnector::connecting(int sockfd) {
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new EvChannel(loop_, sockfd));
    channel_->setWriteCallback(std::bind(&EvConnector::handleWrite, this));  // FIXME: unsafe
    channel_->setErrorCallback(std::bind(&EvConnector::handleError, this));  // FIXME: unsafe

    // channel_->tie(shared_from_this()); is not working,
    // as channel_ is not managed by shared_ptr
    channel_->enableWriting();
}

int EvConnector::removeAndResetChannel() {
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    // Can't reset channel_ here, because we are inside EvChannel::handleEvent
    loop_->queueInLoop(std::bind(&EvConnector::resetChannel, this));  // FIXME: unsafe
    return sockfd;
}

void EvConnector::resetChannel() { channel_.reset(); }

void EvConnector::handleWrite() {
    LOG_TRACE << "EvConnector::handleWrite " << state_;

    if (state_ == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sock_get_error(sockfd);
        if (err) {
            LOG_WARN << "EvConnector::handleWrite - SO_ERROR = " << err << " " << strerror_tl(err);
            retry(sockfd);
        } else if (sock_is_self_connect(sockfd)) {
            LOG_WARN << "EvConnector::handleWrite - Self connect";
            retry(sockfd);
        } else {
            setState(kConnected);
            if (connect_) {
                newConnectionCallback_(sockfd);
            } else {
                sock_close(sockfd);
            }
        }
    } else {
        // what happened?
        assert(state_ == kDisconnected);
    }
}

void EvConnector::handleError() {
    LOG_ERROR << "EvConnector::handleError state=" << state_;
    if (state_ == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sock_get_error(sockfd);
        LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
        retry(sockfd);
    }
}

void EvConnector::retry(int sockfd) {
    sock_close(sockfd);
    setState(kDisconnected);
    if (connect_) {
        LOG_INFO << "EvConnector::retry - Retry connecting to " << serverAddr_.to_ipport() << " in "
                 << retryDelayMs_ << " milliseconds. ";
        loop_->runAfter(retryDelayMs_ / 1000.0,
                        std::bind(&EvConnector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    } else {
        LOG_DEBUG << "do not connect";
    }
}

}  // namespace net

}  // namespace arss
