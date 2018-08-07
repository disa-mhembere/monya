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

#include "NodeView.hpp"
#include "Query.hpp"
#include "TaskQueue.hpp"

namespace monya { namespace container {

    BuildTaskQueue::BuildTaskQueue() {

        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&mutex, &mutex_attr);
    }

    // NOTE: Testing only expensive
    void BuildTaskQueue::print() const {
        std::queue<NodeView*> tmp = tasks; // copy

        while (!tmp.empty()) {
            tmp.front()->print();
            tmp.pop();
        }
    }

    BuildTaskQueue::~BuildTaskQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_mutexattr_destroy(&mutex_attr);
    }

    const bool BuildTaskQueue::has_task() {
        acquire_lock();
        bool _has_task = false; // TODO: could cause errors

        _has_task = !tasks.empty();
        release_lock();
        return _has_task;
    }

    /**
      * Push nodes onto the task queue
      */
    void BuildTaskQueue::enqueue(NodeView** runnables, const size_t nnodes) {
        acquire_lock();
        for (size_t i = 0; i < nnodes; i++)
            tasks.push(runnables[i]);
        release_lock();
    }

    void BuildTaskQueue::enqueue(NodeView* runnable) {
        acquire_lock();
        tasks.push(runnable);
        release_lock();
    }

    NodeView* BuildTaskQueue::dequeue() {
        // TODO: Verify no lock need to check for empty
        if (tasks.empty())
            return NULL;

        acquire_lock();
        NodeView* node = tasks.front();
        tasks.pop();
        release_lock();
        return node;
    }

    void BuildTaskQueue::acquire_lock() {
        int rc = pthread_mutex_lock(&mutex);
        if (rc) throw concurrency_exception("pthread_mutex_lock", rc,
                __FILE__, __LINE__);
    }

    void BuildTaskQueue::release_lock() {
        int rc = pthread_mutex_unlock(&mutex);
        if (rc) throw concurrency_exception("pthread_mutex_unlock", rc,
                __FILE__, __LINE__);
    }

    // TODO: Allow other workers to steal tasks (managed by scheduler)
    bool BuildTaskQueue::steal_task() {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    // QueryTaskQueue
    // Note: Inefficient & used for testing only
    void QueryTaskQueue::print() const {
        std::queue<Query*> tmp = tasks; // copy
        while (!tmp.empty()) {
            tmp.front()->print();
            tmp.pop();
        }
    }
} } // End namespace monya::container
