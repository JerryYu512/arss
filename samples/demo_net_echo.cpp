/**
 * MIT License
 * 
 * Copyright © 2022 <Jerry.Yu>.
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
 * @file demo_net_echo.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2022-01-02
 * 
 * @copyright MIT License
 * 
 */
#include "brsdk/log/logging.hpp"
#include "brsdk/net/event/tcp_server.hpp"
#include "brsdk/net/event/tcp_client.hpp"
#include "brsdk/net/event/event_loop.hpp"
#include "brsdk/str/fmt.hpp"
#include "brsdk/thread/thread.hpp"
#include <chrono>
#include <thread>
#include <functional>

using namespace brsdk;
using namespace brsdk::net;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class EchoServer {
public:
	EchoServer(EventLoop *loop, const Address &addr)
	: server_(loop, addr, "EchoServer") {
		server_.SetConnectionCallback(std::bind(&EchoServer::OnConnection, this, _1));
		server_.SetMessageCallback(std::bind(&EchoServer::OnMessage, this, _1, _2, _3));
	}
	void start(void) {
		server_.start();
	}

private:
	// 有连接时
	void OnConnection(const TcpConnectionPtr& cptr) {
		LOG_INFO << "EchoServer - " << cptr->peer_address().ipport()
				 << " -> " << cptr->local_address().ipport()
				 << " is " << str::Fmt::boolean(cptr->connected()) << "\n";
	}
	// 有消息时
	void OnMessage(const TcpConnectionPtr& cptr, NetBuffer* buf, Timestamp time) {
		auto msg(buf->retrieve_all_string());
		LOG_INFO << cptr->name() << " echo " << msg.size() << " bytes[" << msg << "], data recieve at " << time.toString().c_str() << "\n";
		cptr->send(msg);
	}

	TcpServer server_;
};

void on_connect(const TcpConnectionPtr& conn) {
	LOG_INFO << "EchoClient - " << conn->peer_address().ipport()
			 << " -> " << conn->local_address().ipport()
			 << " is " << str::Fmt::boolean(conn->connected()) << "\n";
	conn->send("hello world");
}

void on_msg(const TcpConnectionPtr& conn, NetBuffer* buf, Timestamp time) {
	static int k = 0;
	auto msg(buf->retrieve_all_string());
	LOG_INFO << conn->name() << " echo " << msg.size() << " bytes[" << msg << "], data recieve at " << time.toString().c_str() << "\n";

	if (k < 10) {
		conn->send("hello world");
		k++;
		// std::this_thread::sleep_for(std::chrono::seconds(1));
	} else {
		conn->GetLoop()->quit();
	}
}

static void echo_client(void)
{
	LOG_INFO << thread::tid();
	EventLoop loop;
	Address server_addr("127.0.0.1", 8009);
	Address local_addr("127.0.0.1", 8010);
	TcpClient client(&loop, server_addr, local_addr, "EchoClient");

	client.SetConnectionCallback(on_connect);
	client.SetMessageCallback(on_msg);

	client.connect();

	LOG_INFO << thread::tid();

	loop.loop();
}

void timer_run(void) {
	static int k = 0;
	if (k < 20) {
		k++;
		LOG_TRACE << "timer run [" << k << "]";
	}
}

int main(void) {
	TimeZone beijing(8 * 3600, "CST");
	Logger::setLogLevel(Logger::TRACE);
	Logger::setTimeZone(beijing);

	LOG_INFO << thread::tid();
	EventLoop loop;
	Address addr(8009);
	EchoServer server(&loop, addr);

	server.start();

	auto cli = std::bind(echo_client);
	thread::Thread cli_t(cli, "echo client");
	cli_t.start();
	loop.RunEvery(5, timer_run);
	loop.loop();

	cli_t.join(nullptr);

	return 0;
}