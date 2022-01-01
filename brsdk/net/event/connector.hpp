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
 * @file connector.hpp
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
#include "event_typedef.hpp"
#include "brsdk/net/socket/address.hpp"
#include "brsdk/mix/noncopyable.hpp"

namespace brsdk {

namespace net {

/**
 * @brief 连接器
 * 
 */
class Connector : noncopyable, public std::enable_shared_from_this<Connector> {
public:
	Connector(EventLoop* loop, const Address& server_addr);
	virtual ~Connector();

	void SetNewConnectionCallback(const ConnectorNewConnectionCallback& cb) {
		newConnectionCallback_ = cb;
	}

	void start(void);
	void restart(void);	// loop中调用
	void stop(void);

	// 获取服务器地址
	const Address& server_address(void) const {
		return server_addr_;
	}
private:
	enum States {
		kDisconnected,
		kConnecting,
		kConnected,
	};

	// 最大重连间隔
	static const int kMaxRetryDelayMs = 30 * 1000;
	// 初始重连间隔时间
	static const int kInitRetryDelayMs = 500;

	// 设置状态
	void set_state(States s) { state_ = s; }
	// 启动
	void StartInLoop(void);
	// 停止
	void StopInLoop(void);
	// 连接,for tcp/udp-connect
	void connect(void);
	void connecting(int sockfd);
	// 发送事件处理
	void HandleWrite(void);
	// 异常事件处理
	void HandleError(void);
	// 重连
	void retry(int sockfd);
	// 移除并重置通道
	int RemoveAndResetChannel(void);
	// 重置通道
	void ResetChannel(void);
	
	EventLoop* loop_;
	Address server_addr_;					///< 服务器地址
	std::atomic_bool connect_;				///< 是否连接
	std::atomic<States> state_;				///< 状态
	std::unique_ptr<EventChannel> channel_;	///< 连接器通道
	ConnectorNewConnectionCallback newConnectionCallback_;
	int retry_delayms_;						///< 重连间隔时间
};

} // namespace net

} // namespace brsdk
