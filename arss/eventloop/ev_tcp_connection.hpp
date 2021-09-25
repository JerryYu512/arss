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
 * @file ev_tcp_connection.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-09-04
 *
 * @copyright MIT License
 *
 */
#pragma once

#include "arss/mix/noncopyable.hpp"
#include "arss/mix/types.hpp"
#include "arss/str/string_piece.hpp"
#include "ev_addr.hpp"
#include "ev_buffer.hpp"
#include "ev_type.hpp"

#include <netinet/tcp.h>
#include <memory>

namespace arss {

namespace net {

class EvChannel;
class EventLoop;
class EvTcpSocket;

///
/// TCP connection, for both client and server usage.
///
/// This is an interface class, so don't expose too much details.
class EvTcpConnection : noncopyable, public std::enable_shared_from_this<EvTcpConnection> {
public:
    /// Constructs a EvTcpConnection with a connected sockfd
    ///
    /// User should not create this object.
    EvTcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                    const EvInetAddress& localAddr, const EvInetAddress& peerAddr);
    ~EvTcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const EvInetAddress& localAddress() const { return localAddr_; }
    const EvInetAddress& peerAddress() const { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }
    // return true if success.
    bool get_tcp_info(struct tcp_info*) const;
    std::string get_tcp_info_string() const;

    // void send(std::string&& message); // C++11
    void send(const void* message, int len);
    void send(const str::StringPiece& message);
    // void send(EvBuffer&& message); // C++11
    void send(EvBuffer* message);  // this one will swap data
    void shutdown();             // NOT thread safe, no simultaneous calling
    // void shutdownAndForceCloseAfter(double seconds); // NOT thread safe, no simultaneous calling
    void forceClose();
    void forceCloseWithDelay(double seconds);
    void set_nodelay(bool on);
    // reading or not
    void startRead();
    void stopRead();
    bool isReading() const {
        return reading_;
    };  // NOT thread safe, may race with start/stopReadInLoop

    void setContext(const Any& context) { context_ = context; }

    const Any& getContext() const { return context_; }

    Any* getMutableContext() { return &context_; }

    void setConnectionCallback(const EvConnectionCallback& cb) { connectionCallback_ = cb; }

    void setMessageCallback(const EvMessageCallback& cb) { messageCallback_ = cb; }

    void setWriteCompleteCallback(const EvWriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

    void setHighWaterMarkCallback(const EvHighWaterMarkCallback& cb, size_t highWaterMark) {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }

    /// Advanced interface
    EvBuffer* inputBuffer() { return &inputBuffer_; }

    EvBuffer* outputBuffer() { return &outputBuffer_; }

    /// Internal use only.
    void setCloseCallback(const EvCloseCallback& cb) { closeCallback_ = cb; }

    // called when TcpServer accepts a new connection
    void connectEstablished();  // should be called only once
    // called when TcpServer has removed me from its map
    void connectDestroyed();  // should be called only once

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    // void sendInLoop(std::string&& message);
    void sendInLoop(const str::StringPiece& message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();
    // void shutdownAndForceCloseInLoop(double seconds);
    void forceCloseInLoop();
    void setState(StateE s) { state_ = s; }
    const char* stateToString() const;
    void startReadInLoop();
    void stopReadInLoop();

    EventLoop* loop_;
    const std::string name_;
    StateE state_;  // FIXME: use atomic variable
    bool reading_;
    // we don't expose those classes to client.
    std::unique_ptr<EvTcpSocket> socket_;
    std::unique_ptr<EvChannel> channel_;
    const EvInetAddress localAddr_;
    const EvInetAddress peerAddr_;
    EvConnectionCallback connectionCallback_;
    EvMessageCallback messageCallback_;
    EvWriteCompleteCallback writeCompleteCallback_;
    EvHighWaterMarkCallback highWaterMarkCallback_;
    EvCloseCallback closeCallback_;
    size_t highWaterMark_;
    EvBuffer inputBuffer_;
    EvBuffer outputBuffer_;  // FIXME: use list<Buffer> as output buffer.
    Any context_;
    // FIXME: creationTime_, lastReceiveTime_
    //        bytesReceived_, bytesSent_
};

typedef std::shared_ptr<EvTcpConnection> EvTcpConnectionPtr;

}  // namespace net

}  // namespace arss
