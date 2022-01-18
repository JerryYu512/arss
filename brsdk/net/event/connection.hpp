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
 * @file connection.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-07
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <atomic>
#include "brsdk/mix/noncopyable.hpp"
#include "brsdk/mix/types.hpp"
#include "brsdk/str/string_piece.hpp"
#include "brsdk/net/socket/address.hpp"
#include "tcp_sock.hpp"
#include "event_typedef.hpp"

#include <netinet/tcp.h>
#include <memory>
#include <list>

namespace brsdk {

namespace net {

// class Connection : noncopyable, public std::enable_shared_from_this<Connection> {
// public:
// 	virtual ~Connection();
// };

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
				const Address& local_addr, const Address& peer_addr);
	~TcpConnection();

	// 默认接口
	static void DefaultConnectionCallback(const TcpConnectionPtr& conn);
	static void DefaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp t);

	EventLoop* GetLoop(void) const { return loop_; }
	const std::string& name(void) const {
		return name_;
	}
	const Address& local_address(void) const {
		return local_addr_;
	}
	const Address& peer_address(void) const {
		return peer_addr_;
	}
	bool connected(void) const {
		return kConnected == state_;
	}

	bool disconnected(void) const {
		return kDisconnected == state_;
	}
	bool GetTcpInfo(struct tcp_info* info) const;
	std::string GetTcpInfoString(void);
	// 发送
	void send(const void* message, int len);
	void send(const std::string& message);
	void send(NetBuffer* message);
	// 关闭
	void shutdown(void);
	void ForceClose(void);
	void ForceCloseWithDelay(double seconds);
	void SetNoDelay(bool on);
	// 开始读取
	void StartRead(void);
	// 停止读取
	void StopRead(void);
	// 数据读取中
	bool reading(void) const { return reading_; }

	void set_context(const Any& context) {
		context_ = context;
	}
	const Any& context(void) const { return context_; }
	Any* context(void) { return &context_;}

	void SetConnectionCallback(const TcpConnectionCallback& cb) {
		connectionCallback_ = cb;
	}

	void SetMessageCallback(const TcpMessageCallbak& cb) {
		messageCallback_ = cb;
	}

	void SetWriteCompleteCallback(const TcpWriteCompleteCallbak& cb) {
		writeCompleteCallback_ = cb;
	}

	void SetHighWaterMarkCallback(const TcpHighWaterMarkCallbak& cb, size_t high_water_mark) {
		highWaterMarkCallback_ = cb;
		high_water_mark_ = high_water_mark;
	}

	void SetCloseCallback(const TcpCloseCallbak& cb) {
		closeCallback_ = cb;
	}

	NetBuffer* input_buffer(void) { return &input_buffer_; }
	NetBuffer* output_buffer(void) { return &output_buffer_; }

	// 建立先连接,accept成功时
	void ConnectEstablished(void);
	// 断开连接
	void ConnectDestroyed(void);

private:
	enum StateE {
		kDisconnected,
		kConnecting,
		kConnected,
		kDisconnecting,
	};

	// 读事件
	void HandleRead(Timestamp receive_time);
	void HandleWrite(void);
	void HandleClose(void);
	void HandleError(void);
	
	// 发送
	void SendInLoop(const std::string& message);
	void SendInLoop(const void* message, size_t len);
	// 接收
	void StartReadInLoop(void);
	void StopReadInLoop(void);
	// 关闭
	void ShutDownInLoop(void);
	// 强制关闭
	void ForceCloseInLoop(void);
	void set_state(StateE s) { state_ = s; }
	const char* StateToString(void) const;

	EventLoop* loop_;
	const std::string name_;
	std::atomic<StateE> state_;				///< 连接状态
	std::atomic_bool reading_;				///< 数据读取中

	std::unique_ptr<TcpSocket> socket_;		///< 套接字
	std::unique_ptr<EventChannel> channel_;	///< 连接通道
	const Address local_addr_;				///< 本地地址
	const Address peer_addr_;				///< 对端地址
	TcpConnectionCallback connectionCallback_;
	TcpMessageCallbak messageCallback_;
	TcpWriteCompleteCallbak writeCompleteCallback_;
	TcpHighWaterMarkCallbak highWaterMarkCallback_;
	TcpCloseCallbak closeCallback_;
	size_t high_water_mark_;
	NetBuffer input_buffer_;		///< 接收缓冲区
	NetBuffer output_buffer_;		///< 发送缓冲区
	Any context_;					///< 用户数据
	Timestamp creation_time_;		///< 连接创建时间
	Timestamp last_recvive_time_;	///< 上次数据接收使时间
	uint64_t bytes_received_;		///< 已经接收了的字节数
	uint64_t bytes_sended_;			///< 已经发送了的字节数
};

// class UdpConnection : public Connection {
// public:
// // 接受/发送数据需要使用一条一条的，而不是流式的
// };

} // namespace net

} // namespace brsdk
