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

#include "RBTree.hpp"
#include "RBNode.hpp"
#include <random>

#include "sucommon.hpp"

using namespace monya;

int main(int argc, char* argv[]) {
    std::vector<data_t> members {0.10, 0.8, 0.14, 0.6};

    container::RBTree::ptr tree = container::RBTree::create();

    for (std::vector<data_t>::iterator it = members.begin();
            it != members.end(); ++it) {
        tree->insert(new container::RBNode((data_t)*it));
    }

    // Add some numbers in an ad hoc fashion
    //constexpr int NRAND = std::pow(2, 6) - 1;
    std::default_random_engine generator;
    std::uniform_real_distribution<data_t> distribution(0, 5);

    // TODO: Subclass RBTree from BinaryTree then add back
    //container::RBTree::ptr tree2 = container::RBTree::create();

    //test::NodeMapper nm;
    //// Test arbitrary insertion
    //for (auto i = 0; i < NRAND; i++)
        //nm.insert(new container::RBNode(distribution(generator)));
    //nm.print();
    //nm.populate_tree(tree2);


    tree->echo();
    std::cout << "RBTree Test successful!\n\n";
    return EXIT_SUCCESS;
}
