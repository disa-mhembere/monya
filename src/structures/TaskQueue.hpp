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

#ifndef __MONYA_TASK_QUEUE_HPP__
#define __MONYA_TASK_QUEUE_HPP__

#include <vector>

namespace monya { namespace container {
    class NodeView;
    class Query;

    // Interface
    class TaskQueue {
        public:
            virtual const size_t size() const = 0;
            virtual void print() const = 0;
            virtual const bool has_task() = 0;
            virtual bool steal_task() = 0;
            virtual ~TaskQueue() { };
    };

    // Focus to build ths out
    class BuildTaskQueue : TaskQueue {
        private:
            std::vector<container::NodeView*> tasks;
            pthread_mutex_t mutex;
            pthread_mutexattr_t mutex_attr;

        public:
            BuildTaskQueue();

            const size_t size() const override {
                return tasks.size();
            }

            const bool has_task();
            bool steal_task();

            ////////////////////////////////////////////////////////////////////
            void push(container::NodeView** nodes, const size_t nnodes);
            void get_tasks(std::vector<container::NodeView*>& runnables,
                    const size_t max_tasks);
            void enqueue(container::NodeView* runnable);
            void enqueue(container::NodeView** runnables);
            static BuildTaskQueue* cast2(TaskQueue* q) {
                return static_cast<BuildTaskQueue*>(q);
            }
            ////////////////////////////////////////////////////////////////////

            void print() const;

            ~BuildTaskQueue() override;
    };

    class QueryTaskQueue : TaskQueue {
        private:
            std::vector<container::Query*> tasks;

        public:
            QueryTaskQueue() {
            }

            const size_t size() const override {
                return tasks.size();
            }

            void print() const;
    };
} } // End namespace monya::container

#endif
