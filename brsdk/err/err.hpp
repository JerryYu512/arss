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
#include "net_err.hpp"

namespace brsdk {

///< 错误码数据类型
typedef uint64_t berror_t;

///< 错误码
extern __thread berror_t berrno;

///< 错误码宏前缀，BEC: BluseIO Error Code
#define BEC_XXX

/// 成功
#define BEC_SUCCESS 0ull

/**
 * |-------|----------|--------|----------|----------|-----------| 
 * | 4bit  |  4bit    |  4bit  |   4bit   |  24bit   |   24bit   |
 * | 标识码 |  类别码  | 异常码  |  保留码  |  编号数   |  错误码    |
 * |-------|----------|--------|----------|----------|-----------| 
 */

////////////////////////////////////////////////////////
/// 标识码，4bit，标识的是错误的来源
///////////////////////////////////////////////////////
/**
 * A类：全零，标识openAPI，openAPI的报文中使用的错误码
 * B类：0x1，标识组件，组件库内部和开放接口使用的错误码
 * C类：0x2，标识应用程序，应用程序内部使用的错误码
 */
// 获取标识码
#define BEC_IDENTIFICATION_CODE(code) (((code) >> 60) & 0x0f)
// 填充标识码
#define BEC_ADD_IDENTIFICATION_CODE(identi, code) ((((identi) & 0x0f) << 60) | ((code) & ~(0xf << 60)))

// openAPI
#define BEC_IDENTI_OPENAPI		0ull
// 组件
#define BEC_IDENTI_COMPONENT	1ull
// 应用程序
#define BEC_IDENTI_APPLICATION	2ull

////////////////////////////////////////////////////////
/// 类别码，4bit，区分应用类型
///////////////////////////////////////////////////////
// 获取类别码
#define BEC_CLASS_CODE(code) (((code) >> 56) & 0x0f)
// 填充标识码
#define BEC_ADD_CLASS_CODE(class, code) ((((class) & 0x0f) << 56) | ((code) & ~(0xf << 56)))

/**
 * 0x0，常规应用
 */
#define BEC_COMMON_APPLICATION 0ull

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
// 获取异常码
#define BEC_EXCEPTION_CODE(code) (((code) >> 52) & 0x0f)
// 添加异常码
#define BEC_ADD_EXCEPTION_CODE(exception, code) ((((exception) & 0x0f) << 52) | ((code) & ~(0xf << 52)))

// 获取严重等级
#define BEC_SEVERE_LEVEL(exception) (((exception) >> 2) & 0x1)
// 添加严重等级
#define BEC_ADD_SEVERE_LEVEL(lv, exception) ((((lv) & 0x1) << 2) | ((exception) & ~(0x1 << 2)))

///< 异常
#define BEC_ABNORMAL_CODE 	1ull
///< 失败
#define BEC_FAILED_CODE 	3ull

////////////////////////////////////////////////////////
/// 编号数，24bit，每组openAPI/组件/应用程序的唯一编号
/// 全0保留
///////////////////////////////////////////////////////
// 获取编号
#define BEC_ID_CODE(code) (((code) >> 24) & 0x0ffffff)
// 添加编号
#define BEC_ADD_ID_CODE(id, code) ((((id) & 0x0ffffff) << 24) | ((code) & ~(0xffffff << 24)))

////////////////////////////////////////////////////////
/// 错误码，24bit，错误/异常的具体信息
///////////////////////////////////////////////////////
/**
 * |--------|---------|---------|--------|
 * |  8bit  |  4bit   |  4bit   |  8bit  |
 * |  领域码 |  主类型  |  次类型  |  具体码 |
 * |--------|---------|---------|--------|
 */
// 获取错误码
#define BEC_ERROR_CODE(code) ((code) & 0x0ffffff)
// 设置错误码
#define BEC_ADD_ERROR_CODE(err, code) ((err) & 0x0ffffff) | ((code) & ~0x0ffffff))

///< 领域码，标识应用内的功能领域
///< 全0保留

// 领域
#define BEC_DOMAIN_CODE(err) (((err) >> 16) & 0xff)
// 添加领域
#define BEC_ADD_DOMAIN_CODE(domain, code) ((((domain) & 0x0ff) << 16) | ((code) & ~(0xff << 16)))

///< 主类型，标识错误的大类
// 0:全0保留
// 1:子系统错误
#define BEC_MAIN_SUBSYS		1ull
// 2:系统错误
#define BEC_MAIN_SYS		2ull
// 3:协议错误
#define BEC_MAIN_PROTOCOL	3ull
// 4:认证错误
#define BEC_MAIN_AUTH		4ull
// 5:网络错误
#define BEC_MAIN_NET		5ull
// 6:设备异常
#define BEC_MAIN_DEVICE		6ull
// 7:参数错误
#define BEC_MAIN_PARAM		7ull
// 8:服务错误
#define BEC_MAIN_SERVICE	8ull
// 9:资源错误
#define BEC_MAIN_SOURCE		9ull
// 15:其他错误
#define BEC_MAIN_OTHER		15ull

// 主类型
#define BEC_MAIN_CODE(err) (((err) >> 12) & 0xf)
// 添加主类型
#define BEC_ADD_MAIN_CODE(main, code) ((((main) & 0x0f) << 12) | ((code) & ~(0xf << 12)))

// 次类型
#define BEC_SUB_CODE(err) (((err) >> 8) & 0xf)
// 添加次类型
#define BEC_ADD_SUB_CODE(sub, code) ((((sub) & 0x0f) << 8) | ((code) & ~(0xf << 8)))

// !!!linux errno，此值时，错误为linux的errno值
// 其他任何子类型都只能从2开始
#define BEC_SUB_LINUX_ERR_CODE		1

// 错误
#define BEC_ERROR_CODE(err) ((err) & 0xff)
// 添加错误
#define BEC_ADD_ERROR_CODE(item, code) (((item) & 0x0ff) | ((code) & ~0xff))

///< 基本常用错误
// 全零保留
// 不支持
#define BEC_COMMON_NOT_SUPPORT			0xff

// 添加linux的errno
uint64_t bec_add_linux_errno(uint64_t code, int no);
uint64_t bec_add_linux_errno(uint64_t code);

/**
 * @brief 获取标识码字符串
 * 
 * @param code 
 * @return const char* 
 */
const char* bec_identi_str(berror_t identi);
// 获取类别码字符串
const char* bec_class_str(berror_t cls);
// 获取异常码字符串
const char* bec_exception_str(berror_t exception);
// 获取严重等级字符串
const char* bec_severe_str(berror_t exception);
// 领域名
const char* bec_domain_str(berror_t exception);

} // namespace brsdk
