/**
 * MIT License
 *
 * Copyright © 2021 <Jerry.Yu>.
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
 * @file any.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <typeindex>

namespace brsdk {

// https://www.cnblogs.com/qicosmos/p/3420095.html

struct Any {
    Any(void) : m_tpIndex(std::type_index(typeid(void))) {}
    Any(const Any& that) : m_ptr(that.Clone()), m_tpIndex(that.m_tpIndex) {}
    Any(Any&& that) : m_ptr(std::move(that.m_ptr)), m_tpIndex(that.m_tpIndex) {}

    //创建智能指针时，对于一般的类型，通过std::decay来移除引用和cv符，从而获取原始类型
    template <typename U, class = typename std::enable_if<
                              !std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
    Any(U&& value)
        : m_ptr(new Derived<typename std::decay<U>::type>(std::forward<U>(value))),
          m_tpIndex(std::type_index(typeid(typename std::decay<U>::type))) {}

    bool IsNull() const { return !bool(m_ptr); }

    template <class U>
    bool Is() const {
        return m_tpIndex == std::type_index(typeid(U));
    }

    //将Any转换为实际的类型
    template <class U>
    U& AnyCast() {
        if (!Is<U>()) {
            throw std::bad_cast();
        }

        auto derived = dynamic_cast<Derived<U>*>(m_ptr.get());
        return derived->m_value;
    }

    Any& operator=(const Any& a) {
        if (m_ptr == a.m_ptr) return *this;

        m_ptr = a.Clone();
        m_tpIndex = a.m_tpIndex;
        return *this;
    }

private:
    struct Base;
    typedef std::unique_ptr<Base> BasePtr;

    struct Base {
        virtual ~Base() {}
        virtual BasePtr Clone() const = 0;
    };

    template <typename T>
    struct Derived : Base {
        template <typename U>
        Derived(U&& value) : m_value(std::forward<U>(value)) {}

        BasePtr Clone() const { return BasePtr(new Derived<T>(m_value)); }

        T m_value;
    };

    BasePtr Clone() const {
        if (m_ptr != nullptr) return m_ptr->Clone();

        return nullptr;
    }

    BasePtr m_ptr;
    std::type_index m_tpIndex;
};

}  // namespace brsdk
