/**
 * MIT License
 * 
 * Copyright © 2021 <wotsen>.
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
 * @file err.hpp
 * @brief 标准错误码
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-18
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <inttypes.h>

namespace brsdk {

///< 错误码数据类型
typedef uint64_t berror_t;

///< 错误码宏前缀，BEC: BluseIO Error Code
#define BEC_XXX

/**
 * |-------|----------|--------|----------|----------|-----------| 
 * | 4bit  |  4bit    |  4bit  |   4bit   |  24bit   |   24bit   |
 * | 标识码 |  类别码   | 异常码  |  保留码   |  编号数   |  错误码    |
 * |-------|----------|--------|----------|----------|-----------| 
 */

// 组件|常规|失败|保留|1|用户登录|网络错误|拒绝连接|
// 0001|0000|0011|0000|000000000000000000000001|00000001|0001|0000|000000001|

////////////////////////////////////////////////////////
/// 标识码，4bit，标识的是错误的来源
///////////////////////////////////////////////////////
/**
 * A类：全零，标识openAPI，openAPI的报文中使用的错误码
 * B类：0x1，标识组件，组件库内部和开放接口使用的错误码
 * C类：0x2，标识应用程序，应用程序内部使用的错误码
 */
#define BEC_IDENTIFICATION_CODE

// openAPI
#define BEC_IDENTI_OPENAPI		0
// 组件
#define BEC_IDENTI_COMPONENT	1
// 应用程序
#define BEC_IDENTI_APPLICATION	2

////////////////////////////////////////////////////////
/// 类别码，4bit，区分应用类型
///////////////////////////////////////////////////////
/**
 * 0x0，常规应用
 */
#define BEC_

////////////////////////////////////////////////////////
/// 异常码，4bit，区分异常或者失败
///////////////////////////////////////////////////////
/**
 * 低两位标识异常或者失败
 * 00 不区分异常或者失败
 * 01 异常(不识别的错误)
 * 11 失败(处理失败)
 * 
 * 次高位为0标识非严重错误，1标识严重错误
 * 最高位保留
 */
#define BRSDK_ERR_CODE_CATEGORY_CODE(code) (((code) >> 16) & 0xf)

///< 异常/失败码
#define BRSDK_ERR_CODE_ABNORMAL_FAILED_CODE(code) ((code) & 0xffff)

////////////////////////////////////////////////////////
/// 编号数，24bit，每组openAPI/组件/应用程序的唯一编号
/// 全0保留
///////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/// 错误码，24bit，错误/异常的具体信息
///////////////////////////////////////////////////////
/**
 * |--------|---------|---------|--------|
 * |  8bit  |  4bit   |  4bit   |  8bit  |
 * |  领域码 |  主类型  |  次类型  |  具体码 |
 * |--------|---------|---------|--------|
 */

///< 领域码，标识应用内的功能领域
///< 全0保留

///< 主类型，标识错误的大类
// 0:全0保留
// 1:子系统错误
// 2:系统错误
// 3:协议错误
// 4:认证错误
// 5:网络错误
// 6:设备异常
// 7:参数错误
// 8:服务错误
// 9:资源错误
// 15:其他错误

/// 成功
#define BEC_SUCCESS 0
/// 主类型
#define BRSDK_ERR_CODE_MAIN_TYPE(code) (((code) >> 12) & 0xf)
/// 来源子系统错误
#define BRSDK_ERR_CODE_MAIN_TYPE_1 0
/// 标准协议错误
#define BRSDK_ERR_CODE_MAIN_TYPE_2 1
/// 系统内错误
#define BRSDK_ERR_CODE_MAIN_TYPE_3 2

/// 主类型1-认证错误
#define BRSDK_ERR_CODE_MAIN_TYPE_1_SUB_AUTH 0
/// 主类型1-网络错误
#define BRSDK_ERR_CODE_MAIN_TYPE_1_SUB_NET 1
/// 主类型1-数据库错误
#define BRSDK_ERR_CODE_MAIN_TYPE_1_SUB_DB 2
/// 主类型1-系统错误
#define BRSDK_ERR_CODE_MAIN_TYPE_1_SUB_SYS 3
/// 主类型1-设备错误
#define BRSDK_ERR_CODE_MAIN_TYPE_1_SUB_DEV 4

/// 主类型3-参数错误
#define BRSDK_ERR_CODE_MAIN_TYPE_3_SUB_ARG 0
/// 主类型3-服务错误
#define BRSDK_ERR_CODE_MAIN_TYPE_3_SUB_SRV 1
/// 主类型3-资源错误
#define BRSDK_ERR_CODE_MAIN_TYPE_3_SUB_SRC 2
/// 主类型3-其他错误
#define BRSDK_ERR_CODE_MAIN_TYPE_3_SUB_OTH 3

/**
 * @brief 通用错误
 * 
 */
typedef enum {
    E_BRSDK_SUCCESS      		= BRSDK_ERR_CODE_SUCCESS,		///< 成功
    E_BRSDK_INTERNAL     		= 1,							///< 内部错误 
	E_BRSDK_INVALID_CALL			= 2,							///< 调用流程错误
	E_BRSDK_PARAMS_INVALID		= 3,							///< 参数非法
	E_BRSDK_BUFFER_TOOL_SAMLL	= 4,							///< 缓冲区大小不足
	E_BRSDK_DATA_LEN_RANGE		= 5,							///< 数据大小超过范围
	E_BRSDK_MEMORY				= 6,							///< 内存异常
	E_BRSDK_JSON_INVALID			= 7,							///< 非法的json数据
	E_BRSDK_JSON_FORMAT			= 8,							///< json数据有误
	E_BRSDK_QUEUE_FULL			= 9,							///< 消息队列已满
	E_BRSDK_NOT_SUPPORT			= 10,							///< 不支持
} ERR_CODE_COMMON_NO;

/**
 * @brief 网络错误
 * 
 */
typedef enum {
	E_BRSDK_NET_CREATE			= 1,							///< 创建socket失败
	E_BRSDK_NET_CONNECT			= 2,							///< 网络连接失败
	E_BRSDK_NET_DISCONNECTED		= 3,							///< 网络断开连接
	E_BRSDK_TRANSMIT				= 4,							///< 数传传输失败
	E_BRSDK_TRANS_TIMEOUT		= 5,							///< 数传传输超时
	E_BRSDK_NET_DNS				= 6,							///< 域名解析失败
} ERR_CODE_NET_NO;

} // namespace brsdk
