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

#include "Scheduler.hpp"
#include "NodeView.hpp"
#include "WorkerThread.hpp"
#include "TaskQueue.hpp"

#if 0
#include <thread>
#include <chrono>
#endif

// TODO: Implement guided by backing this off
#define MIN_TASKS_PT 8

namespace monya { namespace container {

    // One per tree
    Scheduler::Scheduler(unsigned _fanout, depth_t _max_depth,
            tree_t _tree_id, const unsigned nthread, const int _numa_id):
        fanout(_fanout), max_levels(_max_depth),
        current_level(0), tree_id(_tree_id), numa_id(_numa_id) {

            pthread_mutexattr_init(&mutex_attr);
            pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
            pthread_mutex_init(&mutex, &mutex_attr);
            pthread_cond_init(&cond, NULL);
            pending_threads = nthread;

            printf("Launching scheduler %u with %u nthreads!\n",
                    tree_id, nthread);

            for (unsigned tid = 0; tid < nthread; tid++) {
                threads.push_back(new WorkerThread(numa_id, tid));
                threads.back()->set_parent_cond(&cond);
                threads.back()->set_parent_pending_threads(&pending_threads);
                threads.back()->init();
            }

            wait4completion(); // Wait for threads to init
        }

    void Scheduler::schedule(NodeView* node) {
        pthread_mutex_lock(&mutex);

        if (current_level + 1 > nodes.size()) {
            //nodes[current_level] = std::vector<NodeView*>{ node };
            nodes.push_back(std::vector<NodeView*>{ node });
        } else {
            nodes[current_level].push_back(node);
        }

        if (level_is_full(current_level)) {
            current_level++;
            pthread_mutex_unlock(&mutex);

            // TODO: We don't need to get all nodes before running
            run_level(current_level-1);
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }

    void Scheduler::run_level(const depth_t level) {
        printf("\nRunning level: %lu\n", level);

        std::vector<NodeView*> level_nodes = nodes[level];

        // All worker thread created and in waiting state initially
        assert(level_nodes.size());

        cunsigned tid(threads.size());
        size_t task_index = 0;
        size_t ntasks;

        // circular buffer the threads
        while (true) {
            ntasks = task_index + MIN_TASKS_PT < level_nodes.size() + 1 ?
                MIN_TASKS_PT : level_nodes.size() - task_index;

                threads[tid.get()]->get_task_queue()->enqueue(
                        &level_nodes[task_index], ntasks);
                tid.inc();
                task_index += ntasks;
                if (task_index == level_nodes.size())
                    break;
        }

        // Run nodes in current level
        printf("All nodes in level: %lu given to workers\n", level);
        wake4run(BUILD);
        wait4completion(); // TODO: Level-wise barrier not necessary
#if 0
        std::this_thread::sleep_for(std::chrono::seconds(1));
        printf("NODES AFTER:\n");
        for (auto node : level_nodes)
            node->print();
#endif
    }

    void Scheduler::wake4run(const ThreadState_t state) {
        for (auto thread : threads)
            thread->wake(state);
    }

    void Scheduler::wait4completion() {
        int rc = pthread_mutex_lock(&mutex);
        if (rc)
            throw concurrency_exception("pthread_mutex_lock", rc,
                    __FILE__, __LINE__);

        while (pending_threads != 0) {
            pthread_cond_wait(&cond, &mutex); // Used to wake up scheduler
        }

        rc = pthread_mutex_unlock(&mutex);
        if (rc)
            throw concurrency_exception("pthread_mutex_unlock", rc,
                    __FILE__, __LINE__);
    }

    void Scheduler::destroy_threads() {
        wake4run(EXIT); // First alert pthreads that threads are dead

        // Deallocate structures representing threads
        for (size_t tid = 0; tid < threads.size(); tid++)
            delete(threads[tid]);
    }

    Scheduler::~Scheduler() {
        pthread_mutex_destroy(&mutex);
        pthread_mutexattr_destroy(&mutex_attr);
        pthread_cond_destroy(&cond);
        destroy_threads();
    }
} } // End namespace monya::container
