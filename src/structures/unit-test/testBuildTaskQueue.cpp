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

#include "../../structures/BinaryNode.hpp"
#include "../TaskQueue.hpp"

using namespace monya;

namespace {
class TestNode : public container::BinaryNode {

    public:
        using container::BinaryNode::BinaryNode;

        TestNode() {
            parent = left = right = NULL;
        }

        void run() override {
        }
};
}

int main(int argv, char* argc[]) {
    constexpr size_t NNODES = 100;
    std::vector<TestNode*> nodes;
    container::BuildTaskQueue* tq = new container::BuildTaskQueue();

    // Make nodes Comparator -> i
    for (size_t i = 0; i < NNODES; i++)
        nodes.push_back(new TestNode(i));

    for (TestNode* node : nodes)
        tq->enqueue(node);

    assert(nodes.size() == tq->size());

    int i = 0;
    while (tq->has_task()) {
        auto task =tq->dequeue();
        assert(task == nodes[i++]); // address equality
        task->run();
    }

    // Delete task queue
    delete(tq);

    // Delete nodes
    printf("Deleting the nodes ...\n");
    for (size_t i = 0; i < nodes.size(); i++)
        delete(nodes[i]);

    printf("TaskQueue test successful!\n");
    return EXIT_SUCCESS;
}
