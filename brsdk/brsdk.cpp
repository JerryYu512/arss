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
 * @file brsdk.cpp
 * @brief 
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-11
 * 
 * @copyright MIT License
 * 
 */
#include "brsdk.hpp"

#include "configure.h"

namespace brsdk {

// web-logo:https://patorjk.com/software/taag/#p=display&f=Crawford2&t=Type%20Something%20
// python-logo:https://github.com/JerryYu512/art

// isometric3
// slant
// face:
// cute face6

static const char* asciilogo_slant = R"(
    __                         __    __  
   / /_    _____   _____  ____/ /   / /__
  / __ \  / ___/  / ___/ / __  /   / //_/
 / /_/ / / /     (__  ) / /_/ /   / ,<   
/_.___/ /_/     /____/  \__,_/   /_/|_|  
                                         
)";

const char* brsdk_logo(void) {
    return asciilogo_slant;
}

int version(void) {
    return (BRSDK_VERSION_MAJOR << 16) | (BRSDK_VERSION_MINOR << 8) | BRSDK_VERSION_ALTER;
}

const char *version_string(void) {
    return BRSDK_VERSION;
}

uint64_t version_build(void) {
    return BRSDK_VERSION_BUILD;
}

bool debug_version(void) {
    return BRSDK_DEBUG;
}

const char *build_time(void) {
    return BRSDK_BUILD_TIME;
}

const char *version_mode(void) {
    return BRSDK_MODE;
}

const char *compile_arch(void) {
    return BRSDK_ARCH;
}

const char *compile_plat(void) {
    return BRSDK_PLAT;
}

const char *compile_processor(void) {
    return BRSDK_PROCESSOR;
}

} // namespace brsdk
