/*
 * Copyright 2017 Neurodata (https://neurodata.io)
 * Written by Disa Mhembere (disa@cs.jhu.edu)
 *
 * This file is part of Monya.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MONYA_WORKER_THREAD_HPP__
#define MONYA_WORKER_THREAD_HPP__

#include <memory>
#include <utility>
#include <atomic>

#include <pthread.h>

#include "ThreadState.hpp"
#include "../common/exception.hpp"

#define INVALID_THD_ID -1
#define MIN_BUILD_TASKS 2 // TODO: Make config

namespace monya {
    namespace container {
        class BuildTaskQueue;
        class NodeView;
    }

class WorkerThread {
protected:
    pthread_t hw_thd;
    unsigned node_id; // Which NUMA node are you on?
    int thd_id;

    pthread_mutex_t state_lock;
    pthread_cond_t cond;
    pthread_mutexattr_t mutex_attr;

    pthread_cond_t* parent_cond;
    std::atomic<unsigned>* parent_pending_threads;
    ThreadState_t state;

    // Unique to building
    container::BuildTaskQueue* task_queue;
    container::NodeView* active_node;

    friend void* callback(void* arg);

    void set_state(const ThreadState_t state) {
        this->state = state;
    }

    void acquire_state_lock();
    void release_state_lock();
    void acquire_task_queue_lock();
    void release_task_queue_lock();

public:
    typedef WorkerThread* raw_ptr;

    WorkerThread(const int node_id, const int  thd_id);
    void start();
    virtual void run();
    virtual void sleep();
    virtual void lock_sleep();
    virtual void wait();
    virtual void join();
    void request_task();

    // Class that
    virtual void set_driver(void* driver);
    virtual void wake(const ThreadState_t state);
    virtual bool try_steal_task();
    virtual const void print_task_queue() const;
    void bind2node_id();

    void test();

    container::BuildTaskQueue* get_task_queue() const {
        return task_queue;
    }

    const ThreadState_t get_state() const {
        return state;
    }

    const int get_thd_id() const {
        return thd_id;
    }

    pthread_mutex_t& get_lock() {
        return state_lock;
    }

    pthread_cond_t& get_cond() {
        return cond;
    }

    unsigned get_node_id() {
        return node_id;
    }

    void set_parent_cond(pthread_cond_t* cond) {
        parent_cond = cond;
    }

    void set_parent_pending_threads(std::atomic<unsigned>* ppt) {
        parent_pending_threads = ppt;
    }

    virtual ~WorkerThread();
};
}
#endif
