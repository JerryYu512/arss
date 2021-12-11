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
 * @file tcp_client.hpp
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
#include "brsdk/mix/noncopyable.hpp"
#include "brsdk/lock/mutex.hpp"
#include "brsdk/net/socket/address.hpp"
#include "brsdk/net/buffer.hpp"
#include "connection.hpp"
#include "event_typedef.hpp"

namespace brsdk {

namespace net {

/**
 * @brief tcp客户端
 * 
 */
class TcpClient : noncopyable {
public:
	TcpClient(EventLoop* loop, const Address& serverAddr, const std::string& nameArgs);
	~TcpClient();

	// 连接
	void connect(void);
	// 断开连接
	void disconnect(void);
	// 停止
	void stop(void);

	// 获取连接句柄
	TcpConnectionPtr connection(void) const {
		MutexLockGuard lock(mutex_);
		return connection_;
	}

	// 获取事件循环句柄
	EventLoop* GetLoop(void) const { return loop_; };
	// 是否允许重连
	bool retry(void) const { return retry_; }
	// 允许重连
	void EnableRetry(void) { retry_ = true; }

	const std::string& name(void) const { return name_; }

	// 设置连接回调接口
	void SetConnectionCallback(TcpConnectionCallback cb) {
		connectionCallback_ = cb;
	}

	// 设置消息接收回调接口
	void SetMessageCallback(TcpMessageCallbak cb) {
		messageCallback_ = cb;
	}

	// 设置发送完成回调接口
	void SetWriteCompleteCallback(TcpWriteCompleteCallbak cb) {
		writeCompleteCallback_ = cb;
	}

private:
	// 新连接，在loop中执行
	void NewConnection(int sockfd);
	// 移除连接，在loop中执行
	void RemoveConnection(const TcpConnectionPtr& conn);

	EventLoop* loop_;			///< 事件循环器
	TcpConnectorPtr connector_;	///< 连接器
	const std::string name_;	///< 名称
	TcpConnectionCallback connectionCallback_;
	TcpMessageCallbak messageCallback_;
	TcpWriteCompleteCallbak writeCompleteCallback_;
	std::atomic_bool retry_;		///< 重连使能
	std::atomic_bool connect_;		///< 是否已经连接
	// 在事件循环线程里执行
	int nextConnId_;
	mutable MutexLock mutex_;
	TcpConnectionPtr connection_ GUARDED_BY(mutex_);	// 连接句柄
};

} // namespace net

} // namespace brsdk
