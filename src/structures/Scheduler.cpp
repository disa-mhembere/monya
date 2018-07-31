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
            pending_threads = 0;

            printf("Creating scheduler with %u threads: \n", nthread);
            for (unsigned tid = 0; tid < nthread; tid++) {
                threads.push_back(new WorkerThread(numa_id, tid));
                threads.back()->set_parent_cond(&cond);
                threads.back()->set_parent_pending_threads(&pending_threads);
                threads.back()->start();
            }
        }

    // TODO: Batch the scheduling the reduce lock contention
    //  //
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

    // TODO: Handoff nodes to threads
    void Scheduler::run_level(const depth_t level) {
        std::cout << "\nRunning level: " << level << "\n";

        std::vector<NodeView*> level_nodes = nodes[level];

#if 0
        // Encodes dependency on levels below
        // Accounts underflow for level = 0
        for (auto node : level_nodes) {
            assert(node->get_depth() <= get_max_depth());
            node->prep();
            node->run();

            // Only spawn if you're not a leaf
            if (!node->is_leaf())
                node->spawn();
        }


#else
        // All worker thread created and in waiting state initially
        size_t i = 0;
        size_t nthread = threads.size();
        for (auto it = level_nodes.begin(); it != level_nodes.end(); ++it) {

            size_t tid = i++ % nthread;
            //printf("Scheduler feeding and waking tid: %lu\n", tid);
            threads[tid]->get_task_queue()->enqueue(*it);
            threads[tid]->wake(BUILD);
        }
        printf("All nodes in level: %lu given to workers\n", level);
#endif
    }

    Scheduler::~Scheduler() {
        pthread_mutex_destroy(&mutex);
        pthread_mutexattr_destroy(&mutex_attr);
        pthread_cond_destroy(&cond);

        for (size_t tid = 0; tid < threads.size(); tid++)
            delete(threads[tid]);
    }
} } // End namespace monya::container
