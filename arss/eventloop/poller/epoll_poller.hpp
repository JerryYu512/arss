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
 * @file epoll_poller.hpp
 * @brief
 * @author Jerry.Yu (jerry.yu512@outlook.com)
 * @version 1.0.0
 * @date 2021-09-10
 *
 * @copyright MIT License
 *
 */
#pragma once

#include <vector>
#include "arss/eventloop/ev_poller.hpp"

struct epoll_event;

namespace arss {

namespace net {

///
/// IO Multiplexing with epoll(4).
///
class EvEPollPoller : public EvPoller {
public:
    EvEPollPoller(EventLoop* loop);
    ~EvEPollPoller() override;

    Timestamp poll(int timeoutMs, EvChannelList* activeChannels) override;
    void updateChannel(EvChannel* channel) override;
    void removeChannel(EvChannel* channel) override;

private:
    static const int kInitEventListSize = 16;

    static const char* operationToString(int op);

    void fillActiveChannels(int numEvents, EvChannelList* activeChannels) const;
    void update(int operation, EvChannel* channel);

    typedef std::vector<struct epoll_event> EventList;

    int epollfd_;
    EventList events_;
};

}  // namespace net

}  // namespace arss
