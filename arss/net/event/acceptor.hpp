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
 * @file acceptor.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-07
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "event_typedef.hpp"
#include "event_channel.hpp"
#include "arss/net/socket/socket.hpp"
#include "arss/net/socket/address.hpp"

namespace arss {

namespace net {

/**
 * @brief tcp接受器
 * 
 */
class Acceptor : noncopyable {
public:
	Acceptor(EventLoop* loop, const Address& listenAddr, bool reuseport);
	~Acceptor();

	void SetNewConnectionCallback(const AcceptNewConnectionCallback& cb) {
		newConnectionCallback_ = cb;
	}

	// 监听
	void listen(void);
	bool listening(void) const { return listening_; }

private:
	// 读事件处理，对于接受器来说只有accept有连接才是读事件
	void HandleRead(void);

	EventLoop* loop_;
	TcpSocket accept_socket_;		///< 接收器套接字
	EventChannel accept_channel_;	///< 接收器通道
	AcceptNewConnectionCallback newConnectionCallback_;
	bool listening_;				///< 是否处于监听
	int idle_fd_;					///< 空闲句柄
};

} // namespace net

} // namespace arss
