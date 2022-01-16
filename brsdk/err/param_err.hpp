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
 * @file param_err.hpp
 * @brief 
 * @author Jerry.Yu (jerry.yu512outlook.com)
 * @version 1.0.0
 * @date 2022-01-15
 * 
 * @copyright MIT License
 * 
 */
#pragma once

// 常规参数错误子类型
#define BEC_SUB_PARAM_NORMAL	2
// json数据错误
#define BEC_SUB_PARAM_JSON		3

///////////////////////////////////////////////
/// 常规参数错误
///////////////////////////////////////////////
// 参数不在范围内
#define BEC_PARAM_OVER_RANGE	1
// 参数类型错误
#define BEC_PARAM_TYPE_ERROR	2
// 参数非法
#define BEC_PARAM_INVALID		3
// 不识别参数
#define BEC_PARAM_NOT_RECOGNIZ	4
// 参数不支持
#define BEC_PARAM_NOT_SUPPORT	5
// 参数过多
#define BEC_PARAM_TOO_MANY		6
// 参数过少
#define BEC_PARAM_TOO_FEW		7
// 数据过长
#define BEC_PARAM_DATA_TOO_LONG	8

///////////////////////////////////////////////
/// json错误
///////////////////////////////////////////////
// format输出错误
#define BEC_PARAM_JSON_FORMAT	1
// parse错误
#define BEC_PARAM_JSON_PARSE	2
// schema校验错误
#define BEC_PARAM_JSON_SCHEMA	3
// key不存在
#define BEC_PARAM_JSON_NO_KEY	4
// 类型不匹配
#define BEC_PARAM_JSON_TYPE_NOT_MATCH		5
