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

#include "../WorkerThread.hpp"
#include "../../structures/BinaryNode.hpp"
#include <random>

#include <cassert>

namespace mc = monya::container;
using namespace monya;

int main(int argv, char* argc[]) {
    constexpr size_t NNODES = 10;
    constexpr unsigned NTHREADS = 10;
    constexpr int NNUMA_NODES = 1;

    std::vector<mc::BinaryNode*> nodes;

    // Make nodes Comparator -> i
    for (size_t i = 0; i < NNODES; i++)
        nodes.push_back(new mc::BinaryNode(i));

    // Conditional to alert this thread to take control
    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);
    std::atomic<unsigned> ppt(0);

    // Make threads
    std::vector<WorkerThread::raw_ptr> threads;
    for (unsigned tid = 0; tid < NTHREADS; tid++) {
        threads.push_back(new WorkerThread(tid % NNUMA_NODES, tid));
        threads.back()->set_parent_cond(&cond);
        threads.back()->set_parent_pending_threads(&ppt);
        threads.back()->start(WAIT);
    }

    // Run test method (sleeps threads on completion)
    printf("Coordinator thread yeilding control ...\n");
    for (WorkerThread* thread : threads)
        thread->test();

    printf("Coordinator thread regaining control ...\n");
#if 0
    // Run test method AGAIN
    for (WorkerThread* thread : threads)
    for (WorkerThread* thread : threads)
#endif

    // Delete the threads (calls join)
    printf("Deallocating the threads ...\n");
    for (size_t tid = 0; tid < threads.size(); tid++)
        delete(threads[tid]);

    // Delete nodes
    printf("Deleting the nodes ...\n");
    for (size_t i = 0; i < nodes.size(); i++)
        delete(nodes[i]);

    pthread_cond_destroy(&cond);

    printf("WorkerThread test successful!\n");
    return EXIT_SUCCESS;
}
