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
 * @file ev_acceptor.hpp
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
#include "ev_channel.hpp"
#include "ev_tcp_sock.hpp"

namespace arss {

namespace net {

class EventLoop;
class EvInetAddress;

///
/// Acceptor of incoming TCP connections.
///
class EvAcceptor : noncopyable {
public:
    using EvNewConnectionCallback = std::function<void(int sockfd, const EvInetAddress&)>;

    EvAcceptor(EventLoop* loop, const EvInetAddress& listenAddr, bool reuseport);
    ~EvAcceptor();

    void setNewConnectionCallback(const EvNewConnectionCallback& cb) { newConnectionCallback_ = cb; }

    void listen();

    bool listening() const { return listening_; }

    // Deprecated, use the correct spelling one above.
    // Leave the wrong spelling here in case one needs to grep it for error messages.
    // bool listenning() const { return listening(); }

private:
    void handleRead();

    EventLoop* loop_;
    EvTcpSocket acceptSocket_;
    EvChannel acceptChannel_;
    EvNewConnectionCallback newConnectionCallback_;
    bool listening_;
    int idleFd_;
};

}  // namespace net

}  // namespace arss
