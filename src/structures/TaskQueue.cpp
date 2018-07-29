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

    void BuildTaskQueue::print() const {
        for (container::NodeView* node : tasks) {
            node->print();
        }
    }

    BuildTaskQueue::~BuildTaskQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_mutexattr_destroy(&mutex_attr);
    }

    // TODO: Test -- Thread safe
    const bool BuildTaskQueue::has_task() {
        int rc;
        bool _has_task = false; // TODO: could cause errors

        rc = pthread_mutex_lock(&mutex);
        if (rc) perror("pthread_mutex_lock");

        _has_task = !tasks.empty();

        rc = pthread_mutex_unlock(&mutex);
        if (rc) perror("pthread_mutex_unlock");

        return _has_task;
    }

    /**
      * Push nodes onto the task queue
      * TODO: Only one thread can push onto the task queue
      */
    void BuildTaskQueue::push(container::NodeView** nodes, const size_t nnodes) {
        for (size_t i = 0; i < nnodes; i++)
            tasks.push_back(nodes[i]);
    }

    /**
      * Allow worker threads to get tasks
      * TODO: Set the max number of tasks a thread can take at once
      * TODO: Remove copy of NodeView* structs
      * TODO: Each thread gets tasks serially
      */
    void BuildTaskQueue::get_tasks(std::vector<container::NodeView*>& runnables,
            const size_t max_tasks) {
        size_t obtained_tasks = 0;

        while(obtained_tasks < max_tasks && has_task()) {
            // TODO: Verify concurrency bugs
            runnables.push_back(tasks.back());
            tasks.pop_back(); // TODO: optimize for multiple removals
            obtained_tasks++;
        }
    }

    void BuildTaskQueue::enqueue(container::NodeView* runnable) {
        // FIXME: now
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    void BuildTaskQueue::enqueue(container::NodeView** runnables) {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    // Allow other worker to steal tasks
    bool BuildTaskQueue::steal_task() {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    // QueryTaskQueue
    void QueryTaskQueue::print() const {
        for (container::Query* node : tasks) {
            node->print();
        }
    }
} }
