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
 * @file tcp_client.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-11-20
 * 
 * @copyright MIT License
 * 
 */
#include "tcp_client.hpp"
#include "event_log.hpp"
#include "event_channel.hpp"
#include "connector.hpp"
#include "connection.hpp"
#include "event_loop.hpp"

namespace brsdk {

namespace net {

namespace {

using ConnectorPtr = std::shared_ptr<Connector>;

void _RemoveConnection(EventLoop* loop, const TcpConnectionPtr& conn)
{
  loop->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
}

void _RemoveConnector(const ConnectorPtr& connector)
{
  //connector->
}

}

TcpClient::TcpClient(EventLoop* loop, const Address& serverAddr, const std::string& nameArgs)
	: loop_(loop),
	  connector_(new Connector(loop, serverAddr)),
	  name_(nameArgs),
	  connectionCallback_(TcpConnection::DefaultConnectionCallback),
	  messageCallback_(TcpConnection::DefaultMessageCallback),
	  retry_(false),
	  connect_(true),
	  nextConnId_(1) {
	connector_->SetNewConnectionCallback(std::bind(&TcpClient::NewConnection, this, _1));
	LOG_INFO << "TcpClient::TcpClient [" << name_
			 << "] - connector " << get_pointer(connector_);
}
TcpClient::~TcpClient() {
	LOG_INFO << "TcpClient::~TcpClient [" << name_
			 << "] - connector " << get_pointer(connector_);
	TcpConnectionPtr conn;
	bool unique = false;
	{
		MutexLockGuard lock(mutex_);
		unique = connection_.unique();
		conn = connection_;
	}

	if (conn) {
		TcpCloseCallbak cb = std::bind(&_RemoveConnection, loop_, _1);
		loop_->RunInLoop(std::bind(&TcpConnection::SetCloseCallback, conn, cb));
		if (unique) {
			conn->ForceClose();
		}
	} else {
		connector_->stop();
		loop_->RunAfter(1, std::bind(&_RemoveConnector, connector_));
	}
}

// 连接
void TcpClient::connect(void) {
	LOG_INFO << "TcpClient::connect[" << name_ << "] - connecting to "
			 << connector_->server_address().ipport();
	connect_ = true;
	connector_->start();
}

// 断开连接
void TcpClient::disconnect(void) {
	connect_ = false;
	{
		MutexLockGuard lock(mutex_);
		if (connection_) {
			connection_->shutdown();
		}
	}
}

// 停止
void TcpClient::stop(void) {
	connect_ = false;
	connector_->stop();
}

// 新连接，在loop中执行
void TcpClient::NewConnection(int sockfd) {
	loop_->AssertInLoopThread();
	sock_addr_t addr;
	sock_get_peer_name(sockfd, addr);
	Address peer_addr(addr);
	char buf[64] = "";
	snprintf(buf, sizeof(buf), ":%s#%d", peer_addr.ipport().c_str(), nextConnId_);
	nextConnId_++;
	std::string conn_name = name_ + buf;

	memset(&addr, 0, sizeof(addr));
	sock_get_name(sockfd, addr);
	Address local_addr(addr);

	TcpConnectionPtr conn(new TcpConnection(loop_,
											conn_name,
											sockfd,
											local_addr,
											peer_addr));
	conn->SetConnectionCallback(connectionCallback_);
	conn->SetMessageCallback(messageCallback_);
	conn->SetWriteCompleteCallback(writeCompleteCallback_);
	conn->SetCloseCallback(std::bind(&TcpClient::RemoveConnection, this, _1));

	{
		MutexLockGuard lock(mutex_);
		connection_ = conn;
	}
	conn->ConnectEstablished();
}

// 移除连接，在loop中执行
void TcpClient::RemoveConnection(const TcpConnectionPtr& conn) {
	loop_->AssertInLoopThread();
	{
		MutexLockGuard lock(mutex_);
		connection_.reset();
	}
	loop_->QueueInLoop(std::bind(&TcpConnection::ConnectDestroyed, conn));
	if (retry_ && connect_) {
		LOG_INFO << "TcpClient::connect[" << name_ << "] - Reconnecting to "
				 << connector_->server_address().ipport();
		connector_->restart();
	}
}

} // namespace net

} // namespace brsdk
