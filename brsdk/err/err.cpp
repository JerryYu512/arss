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
 * @file err.cpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2022-01-15
 * 
 * @copyright MIT License
 * 
 */
#include "err.hpp"
#include <errno.h>
#include <stddef.h>
#include "brsdk/defs/defs.hpp"

namespace brsdk {

__thread berror_t berrno;

// 添加linux的errno
uint64_t bec_add_linux_errno(uint64_t code, int no)
{
	return BEC_ADD_REAL_ERROR_CODE(BEC_ADD_SUB_CODE(code, BEC_SUB_LINUX_ERR_CODE), no);
}

uint64_t bec_add_linux_errno(uint64_t code)
{
	return BEC_ADD_REAL_ERROR_CODE(BEC_ADD_SUB_CODE(code, BEC_SUB_LINUX_ERR_CODE), errno);
}

/**
 * @brief 获取标识码字符串
 * 
 * @param code 
 * @return const char* 
 */
const char* bec_identi_str(berror_t identi)
{
	static const char* identi_str[] = {
		"openapi",
		"component",
		"application",
	};

	if (identi >= BRSDK_ARRAY_SIZE(identi_str)) {
		return NULL;
	}

	return identi_str[identi];
}

// 获取类别码字符串
const char* bec_class_str(berror_t cls)
{
	static const char* cls_str[] = {
		"common",
	};

	if (cls >= BRSDK_ARRAY_SIZE(cls_str)) {
		return NULL;
	}

	return cls_str[cls];
}

// 获取异常码字符串
const char* bec_exception_str(berror_t exception)
{
	static const char* exp_str[] = {
		"abnormal",
		"failed",
	};

	auto exp = exception & 0x3;

	if (exp >= BRSDK_ARRAY_SIZE(exp_str)) {
		return NULL;
	}

	return exp_str[exp];
}

// 获取严重等级字符串
const char* bec_severe_str(berror_t exception)
{
	static const char* severe_str[] = {
		"not serious"
		"serious",
	};

	auto exp_id = BEC_SEVERE_LEVEL(exception);

	if (exp_id >= BRSDK_ARRAY_SIZE(severe_str)) {
		return NULL;
	}

	return severe_str[exp_id];
}

const char* bec_domain_str(berror_t err)
{
	static const char* domain_str[] = {
		"subsystem",
		"system",
		"protocol",
		"authentication",
		"network",
		"device",
		"parameter",
		"service",
		"source",
		// "other",
	};

	auto idx = BEC_DOMAIN_CODE(err);

	if (idx >= BRSDK_ARRAY_SIZE(domain_str)) {
		return "other";
	}

	return domain_str[idx];
}

}
