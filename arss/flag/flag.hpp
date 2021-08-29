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
 * @file flag.h
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-08-15
 * 
 * @copyright MIT License
 * 
 */
#pragma once
#include <string>
#include <stdint.h>
#include <vector>

// co/flag is a library similar to Google's gflags.
// A flag is in fact a global variable, and value can be passed to it
// from command line or from a config file.

namespace arss {

namespace flag {

// Parse command line flags or config file specified by -config.
// Return non-flag elements.
std::vector<std::string> init(int argc, char** argv);

// Initialize with a config file.
void init(const std::string& path);

namespace xx {
enum {
    TYPE_bool,
    TYPE_int32,
    TYPE_int64,
    TYPE_uint32,
    TYPE_uint64,
    TYPE_string,
    TYPE_double
};

struct FlagSaver {
    FlagSaver(const char* type_str, const char* name, const char* value,
              const char* help, const char* file, int line, int type, void* addr);
};
} // namespace xx
} // namespace flag
} // namespace arss

#define _DECLARE_FLAG(type, name) extern type FLG_##name

// Declare a flag.
// DEC_string(s);  ->  extern fastring FLG_s;
#define DEC_bool(name)    _DECLARE_FLAG(bool, name)
#define DEC_int32(name)   _DECLARE_FLAG(int32, name)
#define DEC_int64(name)   _DECLARE_FLAG(int64, name)
#define DEC_uint32(name)  _DECLARE_FLAG(uint32, name)
#define DEC_uint64(name)  _DECLARE_FLAG(uint64, name)
#define DEC_double(name)  _DECLARE_FLAG(double, name)
#define DEC_string(name)  extern std::string FLG_##name

#define _DEFINE_FLAG(type, name, value, help) \
    type FLG_##name = value; \
    static arss::flag::xx::FlagSaver _Sav_flag_##name( \
        #type, #name, #value, help, __FILE__, __LINE__, arss::flag::xx::TYPE_##type, &FLG_##name \
    )

// Define a flag.
// DEF_int32(i, 23, "xxx");  ->  int32 FLG_i = 23
#define DEF_bool(name, value, help)    _DEFINE_FLAG(bool, name, value, help)
#define DEF_int32(name, value, help)   _DEFINE_FLAG(int32, name, value, help)
#define DEF_int64(name, value, help)   _DEFINE_FLAG(int64, name, value, help)
#define DEF_uint32(name, value, help)  _DEFINE_FLAG(uint32, name, value, help)
#define DEF_uint64(name, value, help)  _DEFINE_FLAG(uint64, name, value, help)
#define DEF_double(name, value, help)  _DEFINE_FLAG(double, name, value, help)

#define DEF_string(name, value, help) \
    std::string FLG_##name = value; \
    static arss::flag::xx::FlagSaver _Sav_flag_##name( \
        "string", #name, #value, help, __FILE__, __LINE__, arss::flag::xx::TYPE_string, &FLG_##name \
    )

DEC_string(help);
DEC_string(config);
