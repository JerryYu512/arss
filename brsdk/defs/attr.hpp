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
	#define BRSDK_DEPRECATED __declspec(deprecated)
#elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
	#define BRSDK_DEPRECATED __attribute__((deprecated))
#else
	#define BRSDK_DEPRECATED
#endif

/// 接口导出
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
	#define BRSDK_DLL_EXPORT_API __declspec(dllexport)
	#define BRSDK_DLL_IMPORT_API __declspec(dllimport)
#else
	#if __GNUC__ >= 4
		#define BRSDK_DLL_EXPORT_API __attribute__((visibility ("default")))
		#define BRSDK_DLL_IMPORT_API
	#else
		#define BRSDK_DLL_EXPORT_API
		#define BRSDK_DLL_IMPORT_API
	#endif
#endif

// useage:
// #pragma BRSDK_TODO(something)
#define __BRSDK_STR2__(x) #x
#define __BRSDK_STR1__(x) __BRSDK_STR2__(x)
#define __BRSDK_LOC__ __FILE__ "(" __BRSDK_STR1__(__LINE__) "): "

#if __GNUC__
	#define BRSDK_TODO(msg) message(__BRSDK_LOC__ "TODO: " #msg)
	#define BRSDK_FIXME(msg) message(__BRSDK_LOC__ "FIXME: " #msg)
#else
	#define BRSDK_TODO(msg)
	#define BRSDK_FIXME(msg)
#endif

/// 未使用不警告的宏
#if defined(__GNUC__)
    #define BRSDK_UNUSED(x) __attribute__((unused)) x 
#else
    #define BRSDK_UNUSED(x) x
#endif

// Thread safety annotations {
// https://clang.llvm.org/docs/ThreadSafetyAnalysis.html

// Enable thread safety attributes only with clang.
// The attributes can be safely erased when compiling with other compilers.
#if defined(__clang__) && (!defined(SWIG))
#define THREAD_ANNOTATION_ATTRIBUTE__(x) __attribute__((x))
#else
#define THREAD_ANNOTATION_ATTRIBUTE__(x)  // no-op
#endif

#define CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(capability(x))

#define SCOPED_CAPABILITY THREAD_ANNOTATION_ATTRIBUTE__(scoped_lockable)

#define GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(guarded_by(x))

#define PT_GUARDED_BY(x) THREAD_ANNOTATION_ATTRIBUTE__(pt_guarded_by(x))

#define ACQUIRED_BEFORE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_before(__VA_ARGS__))

#define ACQUIRED_AFTER(...) THREAD_ANNOTATION_ATTRIBUTE__(acquired_after(__VA_ARGS__))

#define REQUIRES(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_capability(__VA_ARGS__))

#define REQUIRES_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(requires_shared_capability(__VA_ARGS__))

#define ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_capability(__VA_ARGS__))

#define ACQUIRE_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(acquire_shared_capability(__VA_ARGS__))

#define RELEASE(...) THREAD_ANNOTATION_ATTRIBUTE__(release_capability(__VA_ARGS__))

#define RELEASE_SHARED(...) THREAD_ANNOTATION_ATTRIBUTE__(release_shared_capability(__VA_ARGS__))

#define TRY_ACQUIRE(...) THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_capability(__VA_ARGS__))

#define TRY_ACQUIRE_SHARED(...) \
    THREAD_ANNOTATION_ATTRIBUTE__(try_acquire_shared_capability(__VA_ARGS__))

#define EXCLUDES(...) THREAD_ANNOTATION_ATTRIBUTE__(locks_excluded(__VA_ARGS__))

#define ASSERT_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_capability(x))

#define ASSERT_SHARED_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(assert_shared_capability(x))

#define RETURN_CAPABILITY(x) THREAD_ANNOTATION_ATTRIBUTE__(lock_returned(x))

#define NO_THREAD_SAFETY_ANALYSIS THREAD_ANNOTATION_ATTRIBUTE__(no_thread_safety_analysis)

// End of thread safety annotations }

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail(int errnum, const char *file, unsigned int line,
                                 const char *function) noexcept __attribute__((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret)                                                     \
    ({                                                                  \
        __typeof__(ret) errnum = (ret);                                 \
        if (__builtin_expect(errnum != 0, 0))                           \
            __assert_perror_fail(errnum, __FILE__, __LINE__, __func__); \
    })

#else  // CHECK_PTHREAD_RETURN_VALUE

#define MCHECK(ret)                     \
    ({                                  \
        __typeof__(ret) errnum = (ret); \
        assert(errnum == 0);            \
        (void)errnum;                   \
    })

#endif  // CHECK_PTHREAD_RETURN_VALUE