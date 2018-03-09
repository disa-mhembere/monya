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

#include <pthread.h>

namespace monya { namespace container {
    template<typename NodeType>
    class Scheduler {

        private:
            std::map<unsigned, std::vector<NodeType*> > nodes;
            pthread_mutex_t mutex;
            pthread_mutexattr_t mutex_attr;


        public:
            Scheduler() {
                pthread_mutexattr_init(&mutex_attr);
                pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
                pthread_mutex_init(&mutex, &mutex_attr);
            }

            void schedule(NodeType* node, unsigned level) {
                pthread_mutex_lock(&mutex);

                auto it = nodes.find(level);
                if (it == nodes.end()) {
                    nodes[level] = std::vector<NodeType*>{node};
                } else {
                    nodes[level].push_back(node);
                }
                pthread_mutex_unlock(&mutex);
            }

            // Handoff nodes to threads
            void run() {
                for (auto it = nodes.begin(); it != nodes.end(); it++) {
                    // TODO
                }
            }

    };
} } // End namespace monya::container
#endif
