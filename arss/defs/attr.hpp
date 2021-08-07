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
 * @file attr.hpp
 * @brief gcc特性
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

/// 弃用的接口
#if defined(_MSC_VER)
	#define ARSS_DEPRECATED __declspec(deprecated)
#elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
	#define ARSS_DEPRECATED __attribute__((deprecated))
#else
	#define ARSS_DEPRECATED
#endif

/// 接口导出
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
	#define ARSS_DLL_EXPORT_API __declspec(dllexport)
	#define ARSS_DLL_IMPORT_API __declspec(dllimport)
#else
	#if __GNUC__ >= 4
		#define ARSS_DLL_EXPORT_API __attribute__((visibility ("default")))
		#define ARSS_DLL_IMPORT_API
	#else
		#define ARSS_DLL_EXPORT_API
		#define ARSS_DLL_IMPORT_API
	#endif
#endif

// useage:
// #pragma ARSS_TODO(something)
#define __ARSS_STR2__(x) #x
#define __ARSS_STR1__(x) __ARSS_STR2__(x)
#define __ARSS_LOC__ __FILE__ "(" __ARSS_STR1__(__LINE__) "): "

#if __GNUC__
	#define ARSS_TODO(msg) message(__ARSS_LOC__ "TODO: " #msg)
	#define ARSS_FIXME(msg) message(__ARSS_LOC__ "FIXME: " #msg)
#else
	#define ARSS_TODO(msg)
	#define ARSS_FIXME(msg)
#endif

/// 未使用不警告的宏
#if defined(__GNUC__)
    #define ARSS_UNUSED(x) __attribute__((unused)) x 
#else
    #define ARSS_UNUSED(x) x
#endif
