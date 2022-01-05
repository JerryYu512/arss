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
 * @file thread.hpp
 * @brief
 * @author wotsen (astralrovers@outlook.com)
 * @version 1.0.0
 * @date 2021-07-24
 *
 * @copyright MIT License
 *
 */
#pragma once
#include <pthread.h>
#include <atomic>
#include <functional>
#include "brsdk/lock/countdownlatch.hpp"
#include "brsdk/mix/types.hpp"

namespace brsdk {

namespace thread {

// 线程函数
typedef std::function<void()> ThreadFunc;

class Thread : noncopyable {
public:
    // fun-任务函数
    // name-线程名
    explicit Thread(ThreadFunc fun, const std::string& name = std::string());
    // fun-任务函数
    // arg-任务参数
    // attr-任务属性
    // name-线程名
    explicit Thread(ThreadFunc fun, pthread_attr_t *attr, const std::string& name = std::string());
    // FIXME: make it movable in C++11
    ~Thread();

    // 启动
    void start();
    // join
    int join(void **ptr=nullptr);
    // detach
    int detach(void);
    // 绑定cpu
    int bind_cpu(int cpu);
    // 发信号
    int signal(int signal);
    // 是否存活
    bool alive(void);
    // 取消线程
    int cancel(void);
    // 获取属性
    int attribute(pthread_attr_t &attr);

    // 是否开启
    bool started() const { return started_; }
    // 线程id
    pid_t tid() const { return tid_; }
    // 线程名
    const std::string& name() const { return name_; }

public:
    /// 静态方法
    // 让出执行
    static int yield(void);
    // 创建了的线程数量
    static int numCreated() { return numCreated_; }
    // 设置线程栈
    static int set_stacksize(pthread_attr_t &attr, size_t size);
    // 设置保护站大小
    static int set_guardsize(pthread_attr_t &attr, size_t size);
    // 设置优先级
    static int set_prio(pthread_attr_t &attr, int prio);
    // 设置调度策略
    static int set_sched(pthread_attr_t &attr, int sched);

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_attr_t *attr_;                ///< 线程属性
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;

    static std::atomic_int32_t numCreated_;
};

}  // namespace thread

}  // namespace brsdk
