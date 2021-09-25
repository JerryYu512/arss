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
 * @file ev_timerId.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <stdint.h>
#include "arss/mix/copyable.hpp"

namespace arss {

namespace net {

class EvTimer;

///
/// An opaque identifier, for canceling Timer.
///
class EvTimerId : copyable {
public:
    EvTimerId() : timer_(NULL), sequence_(0) {}

    EvTimerId(EvTimer* timer, int64_t seq) : timer_(timer), sequence_(seq) {}

    // default copy-ctor, dtor and assignment are okay

    friend class EvTimerQueue;

private:
    EvTimer* timer_;
    int64_t sequence_;
};

}  // namespace net

}  // namespace arss
