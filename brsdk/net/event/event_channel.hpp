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
 * @file event_channel.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-07
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <functional>
#include <memory>
#include <poll.h>
#include "brsdk/mix/noncopyable.hpp"
#include "event_typedef.hpp"
#include "event_channel.hpp"

namespace brsdk {

namespace net {

class EventLoop;

/**
 * @brief 事件通道
 * 
 */
class EventChannel : noncopyable {
public:
	EventChannel(EventLoop* loop, int fd);
	~EventChannel();

	void HandleEvent(Timestamp receive_time);
	void SetReadCallback(ReadEventCallback cb) {
		readCallback_ = cb;
	}
	void SetWriteCallback(EventCallback cb) {
		writeCallback_ = cb;
	}

	void SetCloseCallback(EventCallback cb) {
		closeCallback_ = cb;
	}

	void SetErrorCallback(EventCallback cb) {
		errorCallback_ = cb;
	}

	// 将该通道与其对象管理者联系起来，避免在事件处理时被销毁掉
	// 创建引用
	void tie(const std::shared_ptr<void>&);
	int fd(void) const { return fd_; }
	// 当前关心事件
	int events(void) const { return events_; }
	// 设置当前活动事件
	void set_revents(int revt) { revents_ = revt; }
	// 是否无关心事件
	bool IsNoneEvent(void) const { return events_ == kNoneEvent; }

	// 设置读事件
	void EnableReading(void) {
		events_ |= kReadEvent;
		update();
	}

	// 屏蔽读事件
	void disableReading(void) {
		events_ &= ~kReadEvent;
		update();
	}

	void EnableWriting(void) {
		events_ |= kWriteEvent;
		update();
	}

	void DisableWriting(void) {
		events_ &= ~kWriteEvent;
		update();
	}

	void DisableAll(void) {
		events_ = kNoneEvent;
		update();
	}

	bool iswriting(void) const {
		return events_ & kWriteEvent;
	}
	bool isreading(void) const {
		return events_ & kReadEvent;
	}

	// 通道索引
	int index(void) { return index_; }
	// 设置通道索引
	void set_index(int idx) { index_ = idx; }

	// 当前事件
	std::string ReventsToString(void) const;
	// 关心事件
	std::string EventsToString(void) const;

	// 不允许日志挂载
	void DoNotLogHup(void) { log_hup_ = false; }

	// 所属事件循环
	EventLoop* OwnerLoop(void) {
		return loop_;
	}

	// 移除
	void remove(void);

private:
	// 事件转字符串
	static std::string EventsToString(int fd, int event);
	// 事件更新
	void update(void);
	// 事件处理
	void HandleEventWithGuard(Timestamp receive_time);

	// 空事件
	static const int kNoneEvent = 0;
	// 读事件
	static const int kReadEvent = POLLIN | POLLPRI;
	// 写事件
	static const int kWriteEvent = POLLOUT;

	EventLoop* loop_;	///< 事件循环
	const int fd_;		///< 套接字
	int events_;		///< 用户设置的事件，channel关心的io事件，用户设置
	int revents_;		///< 接收事件类型,epoll/poll,当前活动事件
	int index_;			///< 事件轮询器使用，当前的通道状态
	bool log_hup_;		///< 日志

	std::weak_ptr<void> tie_;	///< 引用
	bool tied_;					///< 创建引用

	bool event_handling_;	///< 事件句柄
	bool added_to_loop_;	///< 添加到事件循环
	ReadEventCallback readCallback_;	///< 读事件回调
	EventCallback writeCallback_;		///< 写事件回调
	EventCallback closeCallback_;		///< 关闭事件回调
	EventCallback errorCallback_;		///< 错误事件回调
};

} // namespace net

} // namespace brsdk
