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

#ifndef MONYA_SCHEDULER_HPP__
#define MONYA_SCHEDULER_HPP__

#include <vector>
#include <cmath>
#include <iostream>
#include <atomic>

#include "ThreadState.hpp"
#include "../common/types.hpp"

namespace monya {
    class WorkerThread;

    namespace container {

    class NodeView;

    class Scheduler {

        private:
            unsigned fanout;
            depth_t max_levels; // Maximum depth of trees permitted
            // Depth of the tree being scheduled
            // FIXME: Modification lock reading ..
            depth_t current_level;
            const tree_t tree_id;
            const int numa_id;
            // Each level has a list of active nodes
            std::vector<std::vector<NodeView*> > nodes;
            std::vector<WorkerThread*> threads;
            pthread_mutex_t mutex;
            pthread_mutexattr_t mutex_attr;
            pthread_cond_t cond;
            std::atomic<unsigned> pending_threads;

        public:
            Scheduler(unsigned fanout, depth_t max_depth,
                    tree_t tree_id, const unsigned nthread, const int numa_id);
            // Have all the nodes in this level been placed in the queue
            bool level_is_full(unsigned level) {
                return std::pow(fanout, level) == nodes[level].size();
            }

            /**
              \brief Add a node to the schedulers list given a particular
              tree id
              */
            void schedule(NodeView* node);

            /**
              \brief Handoff nodes to threads
              */
            void run_level(const depth_t level);
            void wake4run(const ThreadState_t state);
            void wait4completion();
            void destroy_threads();

            const depth_t get_max_depth() const { return max_levels; }
            const depth_t get_current_level() const { return current_level; }
            std::vector<NodeView*>& get_nodes(const unsigned level) {
                return nodes[level];
            }

            const unsigned get_nthread() const {
                return threads.size();
            }



            ~Scheduler();
    };
} } // End namespace monya::container
#endif
