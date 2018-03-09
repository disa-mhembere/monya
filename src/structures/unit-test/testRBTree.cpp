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

namespace mc = monya::container;

int main(int argc, char* argv[]) {
    std::vector<double> members {10, 8, 14, 6};

    mc::RBTree<mc::RBNode<double> >::ptr tree =
        mc::RBTree<mc::RBNode<double> >::create();

    for (std::vector<double>::iterator it = members.begin();
            it != members.end(); ++it) {
        tree->insert(new mc::RBNode<double>((double)*it));
    }

    // Add some numbers in an ad hoc fashion
    constexpr int NRAND = 100;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0, 5);

    mc::RBTree<mc::RBNode<double> >::ptr tree2 =
        mc::RBTree<mc::RBNode<double> >::create();
    mc::RBNode<double>* node = tree2->get_root();

    std::vector<double> members2;
    // Test arbitrary insertion
    for (auto i = 0; i < NRAND; i++) {
        members2.push_back(distribution(generator));
        auto* insert_node = new mc::RBNode<double>(members2.back());
        if (*insert_node > *node) {
            // TODO
        }
    }


    tree->echo();
    std::cout << "RBTree Test successful!\n\n";
    return EXIT_SUCCESS;
}
