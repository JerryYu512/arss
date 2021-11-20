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
 * @file tcp_server.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-10-25
 * 
 * @copyright MIT License
 * 
 */
#include "event_log.hpp"
#include "tcp_server.hpp"
#include "event_channel.hpp"
#include "acceptor.hpp"
#include "connection.hpp"
#include "event_loop.hpp"
#include "event_loop_thread.hpp"
#include "event_loop_thread_pool.hpp"

namespace arss {

namespace net {
	
TcpServer::TcpServer(EventLoop* loop, const Address& listenAddr, const std::string& nameArg, Option option)
	: loop_(loop), 
	  ipport_(listenAddr.ipport()),
	  name_(nameArg),
	  acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
	  threadPool_(new EventLoopThreadPool(loop, name_)),
	  connectionCallback_(TcpConnection::DefaultConnectionCallback),
	  messageCallback_(TcpConnection::DefaultMessageCallback),
	  writeCompleteCallback_(nullptr),
	  threadInitCallback_(nullptr),
	  started_(false),
	  nextConnId_(1) {
	// 新客户端接入时调用
	acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::NewConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
	loop_->AssertInLoopThread();
	LOG_TRACE << "TcpServer::~TcpServer [" << name_ << "] destructing";

	for (auto& item : connections_) {
		TcpConnectionPtr conn(item.second);
		item.second.reset();
		conn->GetLoop()->RunInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
	}
}

void TcpServer::SetThreadNum(int thread_num) {
	threadPool_->set_threadnum(thread_num);
}

void TcpServer::start(void) {
	if (started_ == 0) {
		threadPool_->start(threadInitCallback_);
		loop_->RunInLoop(std::bind(&Acceptor::listen, get_pointer(acceptor_)));
	}
}

void TcpServer::NewConnection(int sockfd, const Address& peerAddr) {
	loop_->AssertInLoopThread();
	EventLoop* io_loop = threadPool_->GetNextLoop();
	char buf[64] = "";

	snprintf(buf, sizeof(buf), "-%s#%d", ipport_.c_str(), nextConnId_);
	nextConnId_++;
	std::string conn_name = name_ + buf;

	LOG_INFO << "TcpServer::NewConnection [" << name_
			 << "] - new connection [" << conn_name
			 << "] from " << peerAddr.ipport();
	sock_addr_t addr;
	sock_get_name(sockfd, addr);
	Address local_addr(addr);

	TcpConnectionPtr conn(new TcpConnection(io_loop,
											conn_name,
											sockfd,
											local_addr,
											peerAddr));
	connections_[conn_name] = conn;
	conn->SetConnectionCallback(connectionCallback_);
	conn->SetMessageCallback(messageCallback_);
	conn->SetWriteCompleteCallback(writeCompleteCallback_);
	conn->SetCloseCallback(std::bind(&TcpServer::RemoveConnection, this, _1));
	io_loop->RunInLoop(std::bind(&TcpConnection::ConnectEstablished, conn));
}

void TcpServer::RemoveConnection(const TcpConnectionPtr& conn) {
	loop_->RunInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
}

void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr& conn) {
	loop_->AssertInLoopThread();
	LOG_INFO << "TcpServer::RemoveConnectionInLoop [" << name_
			 << "] - connection " << conn->name();
	size_t n = connections_.erase(conn->name());
	(void)n;
	EventLoop* io_loop = conn->GetLoop();
	io_loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
}

} // namespace net

} // namespace arss
