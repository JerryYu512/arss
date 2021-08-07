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
 * @file arss.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "arss.hpp"

#include "configure.h"

namespace arss {
    
int version(void) {
    return (ARSS_VERSION_MAJOR << 16) | (ARSS_VERSION_MINOR << 8) | ARSS_VERSION_ALTER;
}

const char *version_string(void) {
    return ARSS_VERSION;
}

uint64_t version_build(void) {
    return ARSS_VERSION_BUILD;
}

bool debug_version(void) {
    return ARSS_DEBUG;
}

const char *build_time(void) {
    return ARSS_BUILD_TIME;
}

const char *version_mode(void) {
    return ARSS_MODE;
}

const char *compile_arch(void) {
    return ARSS_ARCH;
}

const char *compile_plat(void) {
    return ARSS_PLAT;
}

const char *compile_processor(void) {
    return ARSS_PROCESSOR;
}

} // namespace arss
