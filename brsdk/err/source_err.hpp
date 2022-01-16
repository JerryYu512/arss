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
 * @file source_err.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2022-01-15
 * 
 * @copyright MIT License
 * 
 */
#pragma once

// 内存资源错误
#define BEC_SUB_SOURCE_MEMORY		2
// 消息队列
#define BEC_SUB_SOURCE_MSG_QUEUE	3

///////////////////////////////////////////////
/// 内存错误
///////////////////////////////////////////////
// 非法指针
#define BEC_SOURCE_MEM_PTR_INVALID	1
// 内存不足
#define BEC_SOURCE_MEM_NOT_ENOUGH	2
// 内存越界
#define BEC_SOURCE_MEM_OVERSIZE		3
// 内存异常
#define BEC_SOURCE_MEM_EXCEPTION	4
// 数据过长
#define BEC_SOURCE_MME_OVER_RANGE	5

///////////////////////////////////////////////
/// 消息队列
///////////////////////////////////////////////
// 队列满
#define BEC_SOURCE_MQ_FULL			1
// 队列阻塞
#define BEC_SOURCE_MQ_BUSY			2
// 队列不可用
#define BEC_SOURCE_MQ_CAN_NOT_USE	3
