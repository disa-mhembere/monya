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
#include "../common/exception.hpp"
#include "NodeView.hpp"

#ifdef USE_NUMA
#include <numa.h>
#endif

#include <thread>
#include <chrono>

namespace monya {

    WorkerThread::WorkerThread(const int _node_id, const int _thd_id) :
        node_id(_node_id), thd_id(_thd_id), state(WAIT) {

        // Task Queue
        task_queue = new container::BuildTaskQueue();

        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&state_lock, &mutex_attr);
        pthread_cond_init(&cond, NULL);
    }

    void WorkerThread::acquire_state_lock() {
        int rc = pthread_mutex_lock(&state_lock);
        if (rc) throw concurrency_exception("pthread_mutex_lock", rc,
                    __FILE__, __LINE__);
    }

    void WorkerThread::release_state_lock() {
        int rc = pthread_mutex_unlock(&state_lock);
        if (rc) throw concurrency_exception("pthread_mutex_unlock", rc,
                    __FILE__, __LINE__);
    }

    void WorkerThread::lock_sleep() {
        acquire_state_lock();

        (*parent_pending_threads)--;
        set_state(WAIT);

        if (*parent_pending_threads == 0) {
            int rc = pthread_cond_signal(parent_cond); // Wake up parent thread
            if (rc) throw concurrency_exception("pthread_cond_signal", rc,
                    __FILE__, __LINE__);
        }
        release_state_lock();
    }

    // Assumes caller has lock already ... or else ...
    void WorkerThread::sleep() {
        acquire_state_lock();
        set_state(WAIT);
        release_state_lock();

        (*parent_pending_threads)--;

        if (*parent_pending_threads == 0) {
            int rc = pthread_cond_signal(parent_cond); // Wake up parent thread
            if (rc) throw concurrency_exception("pthread_cond_signal", rc,
                    __FILE__, __LINE__);
        }
    }

    void WorkerThread::request_task() {
        active_node = task_queue->dequeue(); // Aquires lock, releases & returns
        if (NULL == active_node) { // No work? Goodnight
            // TODO: Try to steal tasks
            sleep();
        }
    }

    void WorkerThread::run() {
        switch(state) {
            case TEST:
                (*parent_pending_threads)++;
                test();
                break;
            case BUILD:
                request_task();
                // TODO: Combine into some meta-method
                active_node->prep();
                active_node->run();

                // Only spawn if you're not a leaf
                if (!active_node->is_leaf())
                    active_node->spawn();
                break;
            case QUERY:
                // TODO
                throw not_implemented_exception(__FILE__, __LINE__);
                break;
            case WAIT:
                wait();
                break;
            case EXIT:
                throw concurrency_exception("Thread state is EXIT but running!\n",
                        911, __FILE__, __LINE__);
            default:
                throw concurrency_exception("Unknown thread state\n", 911,
                        __FILE__, __LINE__);
        }
        sleep();
    }

    void WorkerThread::test() {
        printf("Test method for thd: %d, NUMA node: %d\n", thd_id, node_id);
        printf("Sleeping for: %d ms\n", thd_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(thd_id*100));
        printf("Thread %d waking back up!\n", thd_id);

        acquire_state_lock();
        set_state(WAIT);
        release_state_lock();
    }

    void* callback(void* arg) {
        WorkerThread* t = static_cast<WorkerThread*>(arg);
#ifdef USE_NUMA
        t->bind2node_id();
#endif

        while (true) { // So we can receive task after task
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


    void WorkerThread::start() {
        printf("Thread %d started ...\n", thd_id);
        (*parent_pending_threads)++;

        set_state(WAIT);
        int rc = pthread_create(&hw_thd, NULL, callback, this);
        if (rc)
            throw concurrency_exception(
                    "Thread creation (pthread_create) failed!", rc,
                    __FILE__, __LINE__);
    }

    void WorkerThread::set_driver(void* driver) {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    void WorkerThread::wake(const ThreadState_t state) {
        acquire_state_lock();

        set_state(state);
        (*parent_pending_threads)++;

        // TODO: Reset/clear data structures?

        release_state_lock();

        // Signal to run callback (end of pthread_cond_wait)
        int rc = pthread_cond_signal(&cond);
        if (rc) throw concurrency_exception("pthread_cond_signal", rc,
                __FILE__, __LINE__);
    }

    // TODO: Implement
    bool WorkerThread::try_steal_task() {
        return false;
    }

    const void WorkerThread::print_task_queue() const {
        task_queue->print();
    }

    void WorkerThread::wait() {
        acquire_state_lock();
        while (state == WAIT) {
            int rc = pthread_cond_wait(&cond, &state_lock);
            if (rc) throw concurrency_exception("pthread_cond_wait", rc,
                    __FILE__, __LINE__);
        }
        release_state_lock();
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
        int rc = pthread_join(hw_thd, NULL);
        if (rc)
            throw concurrency_exception("pthread_join()",
                    rc, __FILE__, __LINE__);

        thd_id = INVALID_THD_ID;
    }

    WorkerThread::~WorkerThread() {
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&state_lock);
        pthread_mutexattr_destroy(&mutex_attr);

        delete(task_queue);
    }
}
