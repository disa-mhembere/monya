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

#include "WorkerThread.hpp"
#include "TaskQueue.hpp"

#ifdef USE_NUMA
#include <numa.h>
#endif

#include <thread>
#include <chrono>

namespace monya {

    WorkerThread::WorkerThread(const int _node_id, const unsigned _thd_id) :
        node_id(_node_id), thd_id(_thd_id), state(WAIT) {

        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&mutex, &mutex_attr);
        pthread_cond_init(&cond, NULL);
    }

    void WorkerThread::lock_sleep() {
        int rc;
        rc = pthread_mutex_lock(&mutex);
        if (rc) perror("pthread_mutex_lock");

        (*parent_pending_threads)--;
        set_thread_state(WAIT);

        if (*parent_pending_threads == 0) {
            rc = pthread_cond_signal(parent_cond); // Wake up parent thread
            if (rc) perror("pthread_cond_signal");
        }
        rc = pthread_mutex_unlock(&mutex);
        if (rc) perror("pthread_mutex_unlock");
    }

    // Assumes caller has lock already ... or else ...
    void WorkerThread::sleep() {
        (*parent_pending_threads)--;
        set_thread_state(WAIT);

        if (*parent_pending_threads == 0) {
            int rc = pthread_cond_signal(parent_cond); // Wake up parent thread
            if (rc) perror("pthread_cond_signal");
        }
    }

    void WorkerThread::run() {
        switch(state) {
            case TEST:
                (*parent_pending_threads)++;
                test();
                lock_sleep();
                break;
            case BUILD:
                ; // TODO
                break;
            case QUERY:
                ; // TODO
                break;
            case WAIT:
                ; // TODO
                // request_task(); // TODO
                break;
            case EXIT:
                throw thread_exception(
                        "Thread state is EXIT but running!\n");
            default:
                throw thread_exception("Unknown thread state\n");
        }
    }

    void WorkerThread::test() {
        printf("Test method for thd: %d, NUMA node: %d\n", thd_id, node_id);
        printf("Sleeping for: %d ms\n", thd_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(thd_id));
        printf("Thread %d waking back up!\n", thd_id);
    }

    void* callback(void* arg) {
        WorkerThread* t = static_cast<WorkerThread*>(arg);
//#ifdef USE_NUMA
#if 0
        t->bind2node_id();
#endif

        while (true) { // So we can receive task after task
            if (t->get_state() == WAIT)
                t->wait();

            if (t->get_state() == EXIT) {// No more work to do
                //printf("Thread %d exiting ...\n", t->thd_id);
                break;
            }

            //printf("Thread %d awake and doing a run()\n", t->thd_id);
            t->run(); // else
        }

        // We've stopped running so exit
        pthread_exit(NULL);
#ifdef _WIN32
        return NULL;
#endif
    }


    void WorkerThread::start(const ThreadState_t state) {
        printf("Thread %d started ...\n", thd_id);
        this->state = state;
        int rc = pthread_create(&hw_thd, NULL, callback, this);
        if (rc)
            throw thread_exception(
                    "Thread creation (pthread_create) failed!", rc);
    }

    void WorkerThread::set_driver(void* driver) {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    void WorkerThread::wake(const ThreadState_t state) {
        int rc;
        rc = pthread_mutex_lock(&mutex);
        if (rc) perror("pthread_mutex_lock");
        set_thread_state(state);

        // TODO: Implement Logic

        rc = pthread_mutex_unlock(&mutex);
        if (rc) perror("pthread_mutex_unlock");

        rc = pthread_cond_signal(&cond);
        if (rc) perror("pthread_cond_signal");
    }

    bool WorkerThread::try_steal_task() {
        return false; // TODO: Not done
    }

    const void WorkerThread::print_task_queue() const {
        task_queue->print();
    }

    void WorkerThread::wait() {
        int rc;
        rc = pthread_mutex_lock(&mutex);
        if (rc) perror("pthread_mutex_lock");

        while (state == WAIT) {
            //printf("Thread %d begin cond_wait\n", thd_id);
            rc = pthread_cond_wait(&cond, &mutex);
            if (rc) perror("pthread_cond_wait");
        }

        pthread_mutex_unlock(&mutex);
    }

    void WorkerThread::bind2node_id() {
#ifdef USE_NUMA
        struct bitmask *bmp = numa_allocate_nodemask();
        numa_bitmask_setbit(bmp, node_id);
        numa_bind(bmp);
        numa_free_nodemask(bmp);
#endif
    }

    void WorkerThread::join() {
        void* join_status;
        int rc = pthread_join(hw_thd, &join_status);
        if (rc)
            throw thread_exception("pthread_join()", rc);

        thd_id = INVALID_THD_ID;
    }

    WorkerThread::~WorkerThread() {
        printf("\n\nDeallocating thread: %d\n", thd_id);
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&mutex);
        pthread_mutexattr_destroy(&mutex_attr);

        if (thd_id != INVALID_THD_ID)
            join();
        printf("\n\nDeallocated thread: %d\n", thd_id);
    }
}
