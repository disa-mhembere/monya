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

//#define VERB 1

namespace monya {

    WorkerThread::WorkerThread(const int _node_id, const int _thd_id) :
        node_id(_node_id), thd_id(_thd_id), state(WAIT) {

#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, ctor****\n", thd_id);
#endif

        // Task Queue
        task_queue = new container::BuildTaskQueue();

        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&state_lock, &mutex_attr);
        pthread_cond_init(&cond, NULL);
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving ctor****\n", thd_id);
#endif
    }

    void WorkerThread::acquire_state_lock(std::string caller) {
        int rc = pthread_mutex_lock(&state_lock);
        if (rc) throw concurrency_exception(caller + std::string(
                    ": pthread_mutex_lock"), rc,
                    __FILE__, __LINE__);
    }

    void WorkerThread::release_state_lock() {
        int rc = pthread_mutex_unlock(&state_lock);
        if (rc) throw concurrency_exception("pthread_mutex_unlock", rc,
                    __FILE__, __LINE__);
    }

    void WorkerThread::wait() {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, wait****\n", thd_id);
#endif
        acquire_state_lock("wait");
        set_state(WAIT);
        dec_ppt();

        if (*parent_pending_threads == 0) {
            int rc = pthread_cond_signal(parent_cond); // Wake up parent thread
            if (rc) throw concurrency_exception("pthread_cond_signal", rc,
                    __FILE__, __LINE__);
        }

        while (state == WAIT) {
            int rc = pthread_cond_wait(&cond, &state_lock);
            if (rc) throw concurrency_exception("pthread_cond_wait", rc,
                    __FILE__, __LINE__);
        }

#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, wait release_state_lock with ppt: %u ****\n",
                thd_id, (unsigned)(*parent_pending_threads));
#endif
        release_state_lock();
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving wait****\n", thd_id);
#endif
    }

    // TODO: Verify -- not like knor
    void WorkerThread::request_task() {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, request_task****\n", thd_id);
#endif
        active_node = task_queue->dequeue(); // Aquires lock, releases & returns
        if (NULL == active_node) { // No work? Goodnight
            // TODO: Try to steal tasks
            //wait();
        }
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving request_task****\n", thd_id);
#endif
    }

    void WorkerThread::run() {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, run****\n", thd_id);
#endif
        switch(state) {
            case TEST:
                test();
                break;
            case BUILD:
                request_task();
                while (active_node) {
                    // TODO: Combine into some meta-method
                    active_node->prep();
                    active_node->run();
                    request_task(); // Keep requesting tasks
                }
                break;
            case QUERY:
                // TODO
                throw not_implemented_exception(__FILE__, __LINE__);
                break;
            case EXIT:
                throw concurrency_exception("Thread state: EXIT but running!\n",
                        911, __FILE__, __LINE__);
            default:
                throw concurrency_exception("Unknown thread state\n", 911,
                        __FILE__, __LINE__);
        }
        wait();
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving run****\n", thd_id);
#endif
    }

    void WorkerThread::test() {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, test****\n", thd_id);
#endif
#if 0
        printf("Sleeping for: %d ms\n", thd_id);
        std::this_thread::sleep_for(std::chrono::milliseconds(thd_id*100));
        printf("Thread %d waking back up!\n", thd_id);
#endif
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving test****\n", thd_id);
#endif
    }

    void* callback(void* arg) {
        WorkerThread* t = static_cast<WorkerThread*>(arg);
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, callback****\n", t->thd_id);
#endif
#if 1
        t->bind2node_id();
#endif
        while (true) { // So we can receive task after task
            if (t->get_state() == WAIT)
                t->wait();
            if (t->get_state() == EXIT) // No more work to do
                break;
            t->run(); // else
        }

        // We've stopped running so exit
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving callback****\n", t->thd_id);
#endif
        pthread_exit(NULL);
        return NULL;
    }

    void WorkerThread::init() {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n\n****t: %d, init****\n", thd_id);
#endif
        int rc = pthread_create(&hw_thd, NULL, callback, this);
        if (rc)
            throw concurrency_exception(
                    "Thread init (pthread_create) failed!", rc,
                    __FILE__, __LINE__);
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n\n****t: %d, leaving init****\n", thd_id);
#endif
    }

    void WorkerThread::set_driver(void* driver) {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    void WorkerThread::wake(const ThreadState_t state) {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, wake****\n", thd_id);
#endif
        acquire_state_lock("wake");

        set_state(state);
        inc_ppt();

        // TODO: Reset/clear data structures?
        release_state_lock();

#if 0
        printf("Waking thread: %d, with State %s & ppt: %u\n", thd_id,
                str::to(state).c_str(),(unsigned)(*parent_pending_threads));
#endif

        // Signal to run callback (end of pthread_cond_wait)
        int rc = pthread_cond_signal(&cond);
        if (rc) throw concurrency_exception("pthread_cond_signal", rc,
                __FILE__, __LINE__);
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving wake in state %s****\n", thd_id,
                str::to(get_state()).c_str());
#endif
    }

    // TODO: Implement
    bool WorkerThread::try_steal_task() {
        return false;
    }

    const void WorkerThread::print_task_queue() const {
        task_queue->print();
    }

    void WorkerThread::bind2node_id() {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, bind2node_id****\n", thd_id);
#endif
#ifdef USE_NUMA
        struct bitmask *bmp = numa_allocate_nodemask();
        numa_bitmask_setbit(bmp, node_id);
        numa_bind(bmp);
        numa_free_nodemask(bmp);
#endif

#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving bind2node_id****\n", thd_id);
#endif
    }

    WorkerThread::~WorkerThread() {
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, dtor****\n", thd_id);
#endif

        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&state_lock);
        pthread_mutexattr_destroy(&mutex_attr);
        delete(task_queue);
#ifdef VERB
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        printf("\n****t: %d, leaving dtor****\n", thd_id);
#endif
    }
}
