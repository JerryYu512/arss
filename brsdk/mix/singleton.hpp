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
 * @file singleton.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include "noncopyable.hpp"
#include <mutex>

namespace brsdk {

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(Type) BRSDK_DISABLE_COPY(Type)
#endif

#define BRSDK_DISABLE_COPY(Class)   \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete;

#define ARS_SINGLETON_DECL(Class) \
public:                           \
    static Class* instance();     \
    static void exitInstance();   \
                                  \
private:                          \
    BRSDK_DISABLE_COPY(Class)       \
    static Class* s_pInstance;    \
    static std::mutex s_mutex;

#define BRSDK_SINGLETON_IMPL(Class)        \
    Class* Class::s_pInstance = NULL;    \
    std::mutex Class::s_mutex;           \
    Class* Class::instance() {           \
        if (s_pInstance == NULL) {       \
            s_mutex.lock();              \
            if (s_pInstance == NULL) {   \
                s_pInstance = new Class; \
            }                            \
            s_mutex.unlock();            \
        }                                \
        return s_pInstance;              \
    }                                    \
    void Class::exitInstance() {         \
        s_mutex.lock();                  \
        if (s_pInstance) {               \
            delete s_pInstance;          \
            s_pInstance = NULL;          \
        }                                \
        s_mutex.unlock();                \
    }

namespace detail {
// This doesn't detect inherited member functions!
// http://stackoverflow.com/questions/1966362/sfinae-to-check-for-inherited-member-functions
template <typename T>
struct has_no_destroy {
    template <typename C>
    static char test(decltype(&C::no_destroy));
    template <typename C>
    static int32_t test(...);
    const static bool value = sizeof(test<T>(0)) == 1;
};
}  // namespace detail

template <typename T>
class Singleton : noncopyable {
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T& instance() {
        pthread_once(&ponce_, &Singleton::init);
        assert(value_ != NULL);
        return *value_;
    }

private:
    static void init() {
        value_ = new T();
        if (!detail::has_no_destroy<T>::value) {
            ::atexit(destroy);
        }
    }

    static void destroy() {
        typedef char T_must_be_complete_type[sizeof(T) == 0 ? -1 : 1];
        T_must_be_complete_type dummy;
        (void)dummy;

        delete value_;
        value_ = NULL;
    }

private:
    static pthread_once_t ponce_;
    static T* value_;
};

template <typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template <typename T>
T* Singleton<T>::value_ = NULL;

} // namespace brsdk
