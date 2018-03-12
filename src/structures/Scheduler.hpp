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

#include "../common/types.hpp"

#include <vector>
#include <unordered_map>
#include <cmath>
#include <iostream>

#include <pthread.h>

namespace monya { namespace container {
    template<typename NodeType>
    class Scheduler {

        private:
            // Depth of tree that's complete
            std::vector<depth_t> completed_levels;
            // Depth of the tree being scheduled
            // FIXME: Modification lock reading ..
            std::vector<depth_t> current_level;
            unsigned fanout;
            // Map is: level, nodes in level
            typedef std::unordered_map<unsigned, std::vector<NodeType*> > ln_t;
            ln_t nodes;
            pthread_mutex_t mutex;
            pthread_mutexattr_t mutex_attr;

        public:
            Scheduler(unsigned fanout=2, tree_t ntree=1) {
                completed_levels.assign(ntree, 0);
                current_level.assign(ntree, 0);
                this->fanout = fanout; // Single fanout for all trees in forest

                // TODO: One per tree?
                pthread_mutexattr_init(&mutex_attr);
                pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
                pthread_mutex_init(&mutex, &mutex_attr);
            }

            // TODO: Remove default value for tree_id
            // Have all the nodes in this level been placed in the queue
            bool is_full(unsigned level, tree_t tree_id=0) {
                // FIXME: Not true for trees > 1
                return std::pow(fanout, level) == nodes[level].size();
            }

            bool is_complete(unsigned level) {
                if (level >= 0) {
                    return true; // FIXME: figure out how to interrogate the trees
                }
                return true;
            }

            // Add a node to the schedulers list given a particular tree id
            // TODO: Batch the scheduling the reduce lock contention
            void schedule(NodeType* node, const tree_t tree_id=0) {
                // TODO: Fix lock contention here
                pthread_mutex_lock(&mutex);
                typename ln_t::iterator it = nodes.find(current_level[tree_id]);
                if (it == nodes.end()) {
                    nodes[current_level[tree_id]] = std::vector<NodeType*>{ node };
                } else {
                    nodes[current_level[tree_id]].push_back(node);
                }

                // TODO: We don't need to get all nodes before running
                if (is_full(current_level[tree_id])) {
                    // TODO: This is true, but on a tree_id basis
                    current_level[tree_id]++;
                    pthread_mutex_unlock(&mutex);

                    run_level(current_level[tree_id]-1, tree_id);
                } else {
                    pthread_mutex_unlock(&mutex);
                }
            }

            // Handoff nodes to threads
            void run_level(const depth_t level, const tree_t tree_id) {
                // TODO: We don't need this either & can go async
                // TODO: Async
                std::cout << "\nRunning level: " << level << "\n";

                // TODO: Will include others when more than 1 tree
                std::vector<NodeType*> level_nodes = nodes[level];

                // Encodes dependency on levels below
                // Accounts underflow for level = 0
                if (level == 0 || completed_levels[tree_id] >= (level-1)) {
                    for (auto it = level_nodes.begin();
                            it != level_nodes.end(); it++) {
                        (*it)->run();
                    }
                    // TODO: Remove completed nodes
                    completed_levels[tree_id]++;
                }
            }

            ~Scheduler() {
                pthread_mutex_destroy(&mutex);
                pthread_mutexattr_destroy(&mutex_attr);
            }
    };
} } // End namespace monya::container
#endif
