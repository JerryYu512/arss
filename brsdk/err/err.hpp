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

namespace brsdk {

/**********************************************************
 * |-------|--------|----------| 
 * | 44bit |  4bit  |   16bit  |
 * | 标识码 | 类别码 | 异常/失败 |
 * |-------|--------|----------| 
 */

///< 标识码，标识不同组件和核心服务，高28位为0
///< A类错误码需要填写标识码，用于组件
///< B类填0，用于openAPI
#define BRSDK_ERR_CODE_IDENTIFICATION_CODE(code) (((code) >> 44) & 0xfff)

///< 类别码
/// 低两位标识异常或者失败
/// 00 不区分异常或失败
/// 01 异常(不识别的错误)
/// 11 失败(处理失败)
/// 次高位为1标识常规错误码，为0标识组件错误码
/// 最高位保留
#define BRSDK_ERR_CODE_CATEGORY_CODE(code) (((code) >> 16) & 0xf)

///< 异常/失败码
#define BRSDK_ERR_CODE_ABNORMAL_FAILED_CODE(code) ((code) & 0xffff)

////////////////////////////////////////////////////////
/// 标识码分类
///////////////////////////////////////////////////////

///< 保留标识码
#define BRSDK_ERR_CODE_RESERVE 0LL
///< 核心服务管理中心
#define ARRD_ERR_CODE_CORE_SERVICE_OPERATION_MANAGEMENT_CENTER 1LL
///< 核心服务后台
#define ARRD_ERR_CODE_CORE_SERVICE_BAKEND 2LL
///< 其他标识码

////////////////////////////////////////////////////////
/// 类别码
///////////////////////////////////////////////////////

/// 不分区异常或错误
#define BRSDK_ERR_CODE_NOT_DIST_ABNORMAL_FAILED(code) ((((code) >> 16) & 3) == 0)
/// 异常
#define BRSDK_ERR_CODE_ABNORMAL(code) ((((code) >> 16) & 3) == 1)
/// 错误
#define BRSDK_ERR_CODE_FAILED(code) ((((code) >> 16) & 3) == 3)
/// 常规错误码
#define BRSDK_ERR_CODE_NORMAL_CODE(code) ((((code) >> 18) & 1) == 1)
/// 组件错误码
#define BRSDK_ERR_CODE_COMPONENT_CODE(code) ((((code) >> 18) & 1) == 0)

/**
 * @brief 合成错误码
 * @param identi 类别码
 * @param type 1-普通错误码，0组件错误码
 * @param af 异常/错误标识
 * @param err 异常/失败码
 */
#define BRSDK_ERR_CODE_MAKE(identi, type, af, err) (\
(((identi) & 0xfff) << 44) \
| (((type) & 1) << 18) \
| (((af) & 3) << 16) \
| ((err) & 0xffff)

////////////////////////////////////////////////////////
/// 错误码
/// |--------|-------|------|
/// |  4bit  | 4bit  | 8bit |
/// | 主类型 | 子类型 | 具体  |
///////////////////////////////////////////////////////

/// 成功
#define BRSDK_ERR_CODE_SUCCESS BRSDK_ERR_CODE_RESERVE
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
