/*
 * Copyright 2018 Neurodata (https://neurodata.io)
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
#include "RBNode.hpp"

namespace mc = monya::container;

class MyNode: public mc::RBNode {
    public:

        MyNode* parent;
        MyNode* left;
        MyNode* right;

        MyNode(unsigned val) {
            comparator = val;
        }

        void spawn() override {
            // TODO: Implement
        }

        void run() override {
            std::cout << comparator << " ..";
        }
};

void serial_test() {
    mc::Scheduler scheduler;
    std::vector<mc::RBNode*> v;
    constexpr unsigned LEVEL = 5;
    const unsigned NNODES = std::pow(2, LEVEL) - 1;
    std::cout << "Running: " << NNODES << " nodes for test\n\n";

    for (unsigned i = 0; i < NNODES; i++)
        v.push_back(new MyNode(i));

    assert(v.size() == NNODES);

    constexpr monya::tree_t tree_id = 0;
    std::cout << "Scheduling and running: ";
    for (unsigned i = 0; i < NNODES; i++) {
        scheduler.schedule(v[i], tree_id);
    }

    std::cout << ".\n";
    v.back()->print();
}

int main(int argc, char* argv[]) {
    serial_test();
    return EXIT_SUCCESS;
}
