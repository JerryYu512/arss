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
 * @file tcp_server.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-07
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <memory>
#include <atomic>
#include "arss/mix/noncopyable.hpp"
#include "arss/lock/mutex.hpp"
#include "arss/net/socket/address.hpp"
#include "arss/net/buffer.hpp"
#include "connection.hpp"
#include "event_typedef.hpp"

namespace arss {

namespace net {

/**
 * @brief tcp服务器
 * 
 */
class TcpServer : noncopyable {
public:
	enum Option {
		kNoReusePort,
		kReusePort,
	};

	TcpServer(EventLoop* loop, const Address& listenAddr, const std::string& nameArg, Option option = kNoReusePort);
	~TcpServer();

	const std::string& ipport(void) const { return ipport_; }
	const std::string& name(void) const { return name_; }
	EventLoop* GetLoop(void) const { return loop_; }

	/**
	 * @brief Set the Thread Num object
	 * @details 新连接将放到loop线程中
	 * @warning 必须在 @c start 之前调用
	 * 
	 * @param thread_num 线程数
	 * - 0 表明所有的I/O都在loop循环线程中，无新线程创建
	 * - 1 表明所有的I/O都在另一个线程内
	 * - N 表明I/O在线程池内
	 */
	void SetThreadNum(int thread_num);
	void SetThreadInitCallback(const ThreadInitCallback& cb) {
		threadInitCallback_ = cb;
	}
	std::shared_ptr<EventLoopThreadPool> threadpool(void) {
		return threadPool_;
	}

	// 开始监听
	void start(void);

	void SetConnectionCallback(const TcpConnectionCallback& cb) {
		connectionCallback_ = cb;
	}

	void SetMessageCallback(const TcpMessageCallbak& cb) {
		messageCallback_ = cb;
	}

	void SetWriteCompleteCallback(const TcpWriteCompleteCallbak& cb) {
		writeCompleteCallback_ = cb;
	}

private:
	void NewConnection(int sockfd, const Address& peerAddr);
	void RemoveConnection(const TcpConnectionPtr& conn);
	void RemoveConnectionInLoop(const TcpConnectionPtr& conn);

	using ConnectionMap = std::map<std::string, TcpConnectionPtr>;

	EventLoop* loop_;			///< 事件循环
	const std::string ipport_;	///< 地址端口
	const std::string name_;	///< 连接名称
	std::unique_ptr<Acceptor> acceptor_;				///< 接受器
	std::shared_ptr<EventLoopThreadPool> threadPool_;	///< 线程池
	TcpConnectionCallback connectionCallback_;			///< 连接回调
	TcpMessageCallbak messageCallback_;
	TcpWriteCompleteCallbak writeCompleteCallback_;
	ThreadInitCallback threadInitCallback_;
	std::atomic_int32_t started_;						///< 是否已经开始
	int nextConnId_;									///< 连接计数
	ConnectionMap connections_;							///< 连接池
};

} // namespace net

} // namespace arss
