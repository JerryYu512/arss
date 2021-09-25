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
 * @file ev_tcp_connection.cpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-19
 *
 * @copyright MIT License
 *
 */
#define CUSTOM_MODULE_NAME "net"
#include "ev_tcp_connection.hpp"
#include "arss/mix/weak_callback.hpp"
#include "arss/log/logging.hpp"
#include "ev_channel.hpp"
#include "ev_tcp_sock.hpp"
#include "evloop.hpp"

namespace arss {

namespace net {

void ev_default_connection_callback(const EvTcpConnectionPtr& conn) {
    LOG_TRACE << conn->localAddress().to_ipport() << " -> " << conn->peerAddress().to_ipport()
              << " is " << (conn->connected() ? "UP" : "DOWN");
    // do not call conn->forceClose(), because some users want to register message callback only.
}

void ev_default_message_callback(const EvTcpConnectionPtr&, EvBuffer* buf, Timestamp) { buf->retrieve_all(); }

EvTcpConnection::EvTcpConnection(EventLoop* loop, const std::string& nameArg, int sockfd,
                             const EvInetAddress& localAddr, const EvInetAddress& peerAddr)
    : loop_(CHECK_NOTNULL(loop)),
      name_(nameArg),
      state_(kConnecting),
      reading_(true),
      socket_(new EvTcpSocket(sockfd)),
      channel_(new EvChannel(loop, sockfd)),
      localAddr_(localAddr),
      peerAddr_(peerAddr),
      highWaterMark_(64 * 1024 * 1024) {
    channel_->setReadCallback(std::bind(&EvTcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(std::bind(&EvTcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&EvTcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&EvTcpConnection::handleError, this));
    LOG_DEBUG << "EvTcpConnection::ctor[" << name_ << "] at " << this << " fd=" << sockfd;
    socket_->set_keepalive(true);
}

EvTcpConnection::~EvTcpConnection() {
    LOG_DEBUG << "EvTcpConnection::dtor[" << name_ << "] at " << this << " fd=" << channel_->fd()
              << " state=" << stateToString();
    assert(state_ == kDisconnected);
}

bool EvTcpConnection::get_tcp_info(struct tcp_info* tcpi) const { return socket_->get_tcp_info(tcpi); }

std::string EvTcpConnection::get_tcp_info_string() const {
    char buf[1024];
    buf[0] = '\0';
    socket_->get_tcp_info_string(buf, sizeof buf);
    return buf;
}

void EvTcpConnection::send(const void* data, int len) {
    send(str::StringPiece(static_cast<const char*>(data), len));
}

void EvTcpConnection::send(const str::StringPiece& message) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(message);
        } else {
            void (EvTcpConnection::*fp)(const str::StringPiece& message) = &EvTcpConnection::sendInLoop;
            loop_->runInLoop(std::bind(fp,
                                       this,  // FIXME
                                       message.as_string()));
            // std::forward<std::string>(message)));
        }
    }
}

// FIXME efficiency!!!
void EvTcpConnection::send(EvBuffer* buf) {
    if (state_ == kConnected) {
        if (loop_->isInLoopThread()) {
            sendInLoop(buf->peek(), buf->readable_bytes());
            buf->retrieve_all();
        } else {
            void (EvTcpConnection::*fp)(const str::StringPiece& message) = &EvTcpConnection::sendInLoop;
            loop_->runInLoop(std::bind(fp,
                                       this,  // FIXME
                                       buf->retrieve_all_string()));
            // std::forward<std::string>(message)));
        }
    }
}

void EvTcpConnection::sendInLoop(const str::StringPiece& message) {
    sendInLoop(message.data(), message.size());
}

void EvTcpConnection::sendInLoop(const void* data, size_t len) {
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (state_ == kDisconnected) {
        LOG_WARN << "disconnected, give up writing";
        return;
    }
    // if no thing in output queue, try writing directly
    if (!channel_->isWriting() && outputBuffer_.readable_bytes() == 0) {
        nwrote = sock_write(channel_->fd(), data, len);
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_) {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
            }
        } else  // nwrote < 0
        {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                LOG_SYSERR << "EvTcpConnection::sendInLoop";
                if (errno == EPIPE || errno == ECONNRESET)  // FIXME: any others?
                {
                    faultError = true;
                }
            }
        }
    }

    assert(remaining <= len);
    if (!faultError && remaining > 0) {
        size_t oldLen = outputBuffer_.readable_bytes();
        if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_ &&
            highWaterMarkCallback_) {
            loop_->queueInLoop(
                std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
        }
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void EvTcpConnection::shutdown() {
    // FIXME: use compare and swap
    if (state_ == kConnected) {
        setState(kDisconnecting);
        // FIXME: shared_from_this()?
        loop_->runInLoop(std::bind(&EvTcpConnection::shutdownInLoop, this));
    }
}

void EvTcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if (!channel_->isWriting()) {
        // we are not writing
        socket_->shutdown_write();
    }
}

// void EvTcpConnection::shutdownAndForceCloseAfter(double seconds)
// {
//   // FIXME: use compare and swap
//   if (state_ == kConnected)
//   {
//     setState(kDisconnecting);
//     loop_->runInLoop(std::bind(&EvTcpConnection::shutdownAndForceCloseInLoop, this, seconds));
//   }
// }

// void EvTcpConnection::shutdownAndForceCloseInLoop(double seconds)
// {
//   loop_->assertInLoopThread();
//   if (!channel_->isWriting())
//   {
//     // we are not writing
//     socket_->shutdown_write();
//   }
//   loop_->runAfter(
//       seconds,
//       makeWeakCallback(shared_from_this(),
//                        &EvTcpConnection::forceCloseInLoop));
// }

void EvTcpConnection::forceClose() {
    // FIXME: use compare and swap
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->queueInLoop(std::bind(&EvTcpConnection::forceCloseInLoop, shared_from_this()));
    }
}

void EvTcpConnection::forceCloseWithDelay(double seconds) {
    if (state_ == kConnected || state_ == kDisconnecting) {
        setState(kDisconnecting);
        loop_->runAfter(
            seconds,
            makeWeakCallback(
                shared_from_this(),
                &EvTcpConnection::forceClose));  // not forceCloseInLoop to avoid race condition
    }
}

void EvTcpConnection::forceCloseInLoop() {
    loop_->assertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting) {
        // as if we received 0 byte in handleRead();
        handleClose();
    }
}

const char* EvTcpConnection::stateToString() const {
    switch (state_) {
        case kDisconnected:
            return "kDisconnected";
        case kConnecting:
            return "kConnecting";
        case kConnected:
            return "kConnected";
        case kDisconnecting:
            return "kDisconnecting";
        default:
            return "unknown state";
    }
}

void EvTcpConnection::set_nodelay(bool on) { socket_->set_nodelay(on); }

void EvTcpConnection::startRead() {
    loop_->runInLoop(std::bind(&EvTcpConnection::startReadInLoop, this));
}

void EvTcpConnection::startReadInLoop() {
    loop_->assertInLoopThread();
    if (!reading_ || !channel_->isReading()) {
        channel_->enableReading();
        reading_ = true;
    }
}

void EvTcpConnection::stopRead() {
    loop_->runInLoop(std::bind(&EvTcpConnection::stopReadInLoop, this));
}

void EvTcpConnection::stopReadInLoop() {
    loop_->assertInLoopThread();
    if (reading_ || channel_->isReading()) {
        channel_->disableReading();
        reading_ = false;
    }
}

void EvTcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void EvTcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();
    if (state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void EvTcpConnection::handleRead(Timestamp receiveTime) {
    loop_->assertInLoopThread();
    int savedErrno = 0;
    ssize_t n = inputBuffer_.read_fd(channel_->fd(), &savedErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = savedErrno;
        LOG_SYSERR << "EvTcpConnection::handleRead";
        handleError();
    }
}

void EvTcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if (channel_->isWriting()) {
        ssize_t n =
            sock_write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readable_bytes());
        if (n > 0) {
            outputBuffer_.retrieve(n);
            if (outputBuffer_.readable_bytes() == 0) {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        } else {
            LOG_SYSERR << "EvTcpConnection::handleWrite";
            // if (state_ == kDisconnecting)
            // {
            //   shutdownInLoop();
            // }
        }
    } else {
        LOG_TRACE << "Connection fd = " << channel_->fd() << " is down, no more writing";
    }
}

void EvTcpConnection::handleClose() {
    loop_->assertInLoopThread();
    LOG_TRACE << "fd = " << channel_->fd() << " state = " << stateToString();
    assert(state_ == kConnected || state_ == kDisconnecting);
    // we don't close fd, leave it to dtor, so we can find leaks easily.
    setState(kDisconnected);
    channel_->disableAll();

    EvTcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    // must be the last line
    closeCallback_(guardThis);
}

void EvTcpConnection::handleError() {
    int err = sock_get_error(channel_->fd());
    LOG_ERROR << "EvTcpConnection::handleError [" << name_ << "] - SO_ERROR = " << err << " "
              << strerror_tl(err);
}

}  // namespace net

}  // namespace arss
