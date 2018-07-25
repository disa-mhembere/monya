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

#include "../structures/NodeView.hpp"
#include "../structures/Query.hpp"
#include "TaskQueue.hpp"

namespace monya {

    void BuildTaskQueue::print() const {
        for (container::NodeView* node : tasks) {
            node->print();
        }
    }

    void QueryTaskQueue::print() const {
        for (container::Query* node : tasks) {
            node->print();
        }
    }
}
