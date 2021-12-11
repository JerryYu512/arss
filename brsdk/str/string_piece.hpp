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
 * @file string_piece.hpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#pragma once

#include <string.h>
#include <iosfwd>  // for ostream forward-declaration
#include "brsdk/mix/types.hpp"

namespace brsdk {

namespace str {

// For passing C-style string argument to a function.
class StringArg  // copyable
{
public:
    StringArg(const char* str) : str_(str) {}

    StringArg(const std::string& str) : str_(str.c_str()) {}

    const char* c_str() const { return str_; }

private:
    const char* str_;
};

class StringPiece {
private:
    const char* ptr_;
    int length_;

public:
    // We provide non-explicit singleton constructors so users can pass
    // in a "const char*" or a "string" wherever a "StringPiece" is
    // expected.
    StringPiece() : ptr_(NULL), length_(0) {}
    StringPiece(const char* str) : ptr_(str), length_(static_cast<int>(strlen(ptr_))) {}
    StringPiece(const unsigned char* str)
        : ptr_(reinterpret_cast<const char*>(str)), length_(static_cast<int>(strlen(ptr_))) {}
    StringPiece(const std::string& str) : ptr_(str.data()), length_(static_cast<int>(str.size())) {}
    StringPiece(const char* offset, int len) : ptr_(offset), length_(len) {}

    // data() may return a pointer to a buffer with embedded NULs, and the
    // returned buffer may or may not be null terminated.  Therefore it is
    // typically a mistake to pass data() to a routine that expects a NUL
    // terminated string.  Use "as_string().c_str()" if you really need to do
    // this.  Or better yet, change your routine so it does not rely on NUL
    // termination.
    const char* data() const { return ptr_; }
    int size() const { return length_; }
    bool empty() const { return length_ == 0; }
    const char* begin() const { return ptr_; }
    const char* end() const { return ptr_ + length_; }

    void clear() {
        ptr_ = NULL;
        length_ = 0;
    }
    void set(const char* buffer, int len) {
        ptr_ = buffer;
        length_ = len;
    }
    void set(const char* str) {
        ptr_ = str;
        length_ = static_cast<int>(strlen(str));
    }
    void set(const void* buffer, int len) {
        ptr_ = reinterpret_cast<const char*>(buffer);
        length_ = len;
    }

    char operator[](int i) const { return ptr_[i]; }

    void remove_prefix(int n) {
        ptr_ += n;
        length_ -= n;
    }

    void remove_suffix(int n) { length_ -= n; }

    bool operator==(const StringPiece& x) const {
        return ((length_ == x.length_) && (memcmp(ptr_, x.ptr_, length_) == 0));
    }
    bool operator!=(const StringPiece& x) const { return !(*this == x); }

#define STRINGPIECE_BINARY_PREDICATE(cmp, auxcmp)                                \
    bool operator cmp(const StringPiece& x) const {                              \
        int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_); \
        return ((r auxcmp 0) || ((r == 0) && (length_ cmp x.length_)));          \
    }
    STRINGPIECE_BINARY_PREDICATE(<, <);
    STRINGPIECE_BINARY_PREDICATE(<=, <);
    STRINGPIECE_BINARY_PREDICATE(>=, >);
    STRINGPIECE_BINARY_PREDICATE(>, >);
#undef STRINGPIECE_BINARY_PREDICATE

    int compare(const StringPiece& x) const {
        int r = memcmp(ptr_, x.ptr_, length_ < x.length_ ? length_ : x.length_);
        if (r == 0) {
            if (length_ < x.length_)
                r = -1;
            else if (length_ > x.length_)
                r = +1;
        }
        return r;
    }

    std::string as_string() const { return std::string(data(), size()); }

    void CopyToString(std::string* target) const { target->assign(ptr_, length_); }

    // Does "this" start with "x"
    bool starts_with(const StringPiece& x) const {
        return ((length_ >= x.length_) && (memcmp(ptr_, x.ptr_, x.length_) == 0));
    }
};

} // namespace str

} // namespace brsdk

// ------------------------------------------------------------------
// Functions used to create STL containers that use StringPiece
//  Remember that a StringPiece's lifetime had better be less than
//  that of the underlying string or char*.  If it is not, then you
//  cannot safely store a StringPiece into an STL container
// ------------------------------------------------------------------

#ifdef HAVE_TYPE_TRAITS
// This makes vector<StringPiece> really fast for some STL implementations
template<> struct __type_traits<ars::sdk::StringPiece> {
  typedef __true_type    has_trivial_default_constructor;
  typedef __true_type    has_trivial_copy_constructor;
  typedef __true_type    has_trivial_assignment_operator;
  typedef __true_type    has_trivial_destructor;
  typedef __true_type    is_POD_type;
};
#endif

// allow StringPiece to be logged
std::ostream& operator<<(std::ostream& o, const brsdk::str::StringPiece& piece);
