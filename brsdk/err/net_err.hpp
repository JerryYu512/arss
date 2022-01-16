/**
 * MIT License
 * 
 * Copyright © 2022 <Jerry.Yu>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS";, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * @file net_err.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2022-01-15
 * 
 * @copyright MIT License
 * 
 */
#pragma once

///< 网络错误

// ip,端口错误
#define BEC_NET_ADDRESS				1
// 地址使用中
#define BEC_NET_ADDRESS_INUSE		2
// 套接字用尽
#define BEC_NET_FULL_SOCK_DESC		3
// 创建socket失败
#define BEC_NET_CREATE				4
// 系统中断
#define BEC_NET_INTERRUPT			5
// 临时不可用，重试
#define BEC_NET_AGAIN				6
// 正忙
#define BEC_NET_BUSY				7
// 管道破裂
#define BEC_NET_PIPE				8
// 网络停止
#define BEC_NET_DOWN				9
// 连接重置
#define BEC_NET_RESET				10
// 缓冲区用完
#define BEC_NET_NOBUF				11
// 未连接
#define BEC_NET_NOT_CONNECT			12
// 不可达
#define BEC_NET_HOST_UNREACH		13
// 连接失败
#define BEC_NET_CONNECT				14
// 拒绝连接
#define BEC_NET_CONNECT_REFUSED		15
// 断开连接
#define BEC_NET_DISCONNECTED		16
// 数据传输失败
#define BEC_NET_TRANSMIT			17
// 网络超时
#define BEC_NET_TIMEOUT				18
// 域名解析失败
#define BEC_NET_DNS_FAILED			19
