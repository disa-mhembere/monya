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
#include "../../utils/utility.hpp"
#include <random>

#include <cassert>

#include <thread>
#include <chrono>

using namespace monya;

namespace {
void init_threads(std::vector<WorkerThread::raw_ptr>& threads,
        const int NTHREADS, pthread_cond_t& cond, std::atomic<unsigned>& ppt ) {

    const int NNUMA_NODES = utils::get_num_nodes();

    for (int tid = 0; tid < NTHREADS; tid++) {
        threads.push_back(new WorkerThread(tid % NNUMA_NODES, tid));
        threads.back()->set_parent_cond(&cond);
        threads.back()->set_parent_pending_threads(&ppt);
        threads.back()->init();
    }
}
}

int main(int argv, char* argc[]) {
    printf("\n\n\n\n***************************************************\n\n\n");
    constexpr int NTHREADS = 16;

    // Conditional to alert this thread to take control
    pthread_cond_t cond;
    pthread_cond_init(&cond, NULL);
    std::atomic<unsigned> ppt(NTHREADS);

    pthread_mutexattr_t mutex_attr;
    pthread_mutex_t mutex;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex, &mutex_attr);

    // Make threads
    std::vector<WorkerThread::raw_ptr> threads;
    init_threads(threads, NTHREADS, cond, ppt);

    // Block for completion
    pthread_mutex_lock(&mutex);
    while (ppt != 0)
        pthread_cond_wait(&cond, &mutex); // Used to wake up scheduler
    pthread_mutex_unlock(&mutex);

    printf("\nCoordinator regains control AFTER INIT calls TEST...\n");

    for (WorkerThread* thread : threads)
        thread->wake(TEST); // Threads fall asleep alone

    // Block for completion
    pthread_mutex_lock(&mutex);
    while (ppt != 0)
        pthread_cond_wait(&cond, &mutex); // Used to wake up scheduler
    pthread_mutex_unlock(&mutex);
    printf("\nCoordinator regains control AFTER TEST calls TEST, again ...\n");

    for (WorkerThread* thread : threads)
        thread->wake(TEST); // Threads fall asleep alone

    // Block for completion
    pthread_mutex_lock(&mutex);
    while (ppt != 0)
        pthread_cond_wait(&cond, &mutex); // Used to wake up scheduler
    pthread_mutex_unlock(&mutex);

    // Delete the threads (calls join)
    printf("Coordinated deallocates the threads ...\n");
    for (size_t tid = 0; tid < threads.size(); tid++) {
        delete(threads[tid]);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutexattr_destroy(&mutex_attr);
    pthread_cond_destroy(&cond);

    printf("WorkerThread test successful!\n");
    return EXIT_SUCCESS;
}
