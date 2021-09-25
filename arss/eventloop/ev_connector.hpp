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
 * @file ev_connector.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once
#include <functional>
#include <memory>
#include "arss/mix/noncopyable.hpp"
#include "ev_addr.hpp"

namespace arss {

namespace net {

class EvChannel;
class EventLoop;

class EvConnector : noncopyable, public std::enable_shared_from_this<EvConnector> {
public:
    using EvNewConnectionCallback = std::function<void(int sockfd)>;

    EvConnector(EventLoop* loop, const EvInetAddress& serverAddr);
    ~EvConnector();

    void setNewConnectionCallback(const EvNewConnectionCallback& cb) { newConnectionCallback_ = cb; }

    void start();    // can be called in any thread
    void restart();  // must be called in loop thread
    void stop();     // can be called in any thread

    const EvInetAddress& serverAddress() const { return serverAddr_; }

private:
    enum States { kDisconnected, kConnecting, kConnected };
    static const int kMaxRetryDelayMs = 30 * 1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s) { state_ = s; }
    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop* loop_;
    EvInetAddress serverAddr_;
    bool connect_;  // atomic
    States state_;  // FIXME: use atomic variable
    std::unique_ptr<EvChannel> channel_;
    EvNewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;
};

}  // namespace net

}  // namespace arss
