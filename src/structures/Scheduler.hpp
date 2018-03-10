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
#include <unordered_map>
#include <cmath>
#include <iostream>

#include <pthread.h>

namespace monya { namespace container {
    template<typename NodeType>
    class Scheduler {

        private:
            unsigned completed_levels;
            unsigned scheduled_levels;
            unsigned fanout;
            // Map is: level, nodes in level
            std::unordered_map<unsigned, std::vector<NodeType*> > nodes;
            pthread_mutex_t mutex;
            pthread_mutexattr_t mutex_attr;

        public:
            Scheduler(unsigned fanout=2) {
                completed_levels = 0;
                scheduled_levels = 0;
                this->fanout = fanout;
                pthread_mutexattr_init(&mutex_attr);
                pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
                pthread_mutex_init(&mutex, &mutex_attr);
            }

            void schedule(NodeType* node, unsigned level) {
                pthread_mutex_lock(&mutex);
#if 0
                auto it = nodes.find(level);
                if (it == nodes.end()) {
                    nodes[level] = std::vector<NodeType*>{ node };
                } else {
                    nodes[level].push_back(node);
                }

                // TODO: We don't need to get all nodes before running
                if (std::pow(fanout, level) == nodes[level].size()) {
                    run_level(level); // Run when all nodes in a level are scheduled
                }

#endif
                pthread_mutex_unlock(&mutex);
            }

            // Handoff nodes to threads
            void run_level(unsigned level) {

                // TODO: We don't need this either & can go async
                // TODO: Async
                std::cout << "Running level: " << level << "\n";
                if (completed_levels == level) {
                    for (auto it = nodes[level].begin();
                            it != nodes.end(); it++) {
                        (*it)->run();
                    }
                    completed_levels++;
                }
            }

            ~Scheduler() {
                pthread_mutex_destroy(&mutex);
                pthread_mutexattr_destroy(&mutex_attr);
            }
    };
} } // End namespace monya::container
#endif
