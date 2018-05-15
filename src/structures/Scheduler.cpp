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

namespace monya { namespace container {

    Scheduler::Scheduler(unsigned fanout, tree_t ntree, depth_t max_depth) {
        max_levels = max_depth;
        current_level.assign(ntree, 0);
        this->fanout = fanout; // Single fanout for all trees in forest

        // TODO: One per tree?
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
        pthread_mutex_init(&mutex, &mutex_attr);
    }

    bool Scheduler::is_complete(unsigned level) {
        if (level >= 0) {
            return true; // FIXME: figure out how to interrogate the trees
        }
        return true;
    }

    // TODO: Batch the scheduling the reduce lock contention
    // TODO: Fix lock contention here
    void Scheduler::schedule(NodeView* node, const tree_t tree_id) {
        pthread_mutex_lock(&mutex);
        typename ln_t::iterator it = nodes.find(current_level[tree_id]);
        if (it == nodes.end()) {
            nodes[current_level[tree_id]] = std::vector<NodeView*>{ node };
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
    void Scheduler::run_level(const depth_t level, const tree_t tree_id) {
        std::cout << "\nRunning level: " << level << "\n";

        // TODO: Will include others when more than 1 tree
        std::vector<NodeView*> level_nodes = nodes[level];

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
    }

    Scheduler::~Scheduler() {
        pthread_mutex_destroy(&mutex);
        pthread_mutexattr_destroy(&mutex_attr);
    }
} } // End namespace monya::container
