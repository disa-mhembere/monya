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

#include "BinaryTree.hpp"
#include "BinaryNode.hpp"
#include <random>

#include "sucommon.hpp"
namespace mc = monya::container;

int main(int argc, char* argv[]) {
    std::vector<double> members {0.10, 0.8, 0.14, 0.6};

    mc::BinaryTree::ptr tree = mc::BinaryTree::create();

    for (std::vector<double>::iterator it = members.begin();
            it != members.end(); ++it) {
        tree->insert(new mc::BinaryNode((double)*it));
    }

    for (auto i : members) {
        // Asserts on failure to find
        mc::BinaryNode* q = new mc::BinaryNode(i);
        mc::BinaryNode* a = tree->find(q);
        assert(*q == *a);
        delete(q);
    }


    //// Add some numbers in an ad hoc fashion
    //constexpr int NRAND = std::pow(2, 6) - 1;
    //std::default_random_engine generator;
    //std::uniform_real_distribution<double> distribution(0, 5);

    //mc::BinaryTree::ptr tree2 = mc::BinaryTree::create();

    //NodeMapper nm;
    //// Test arbitrary insertion
    //for (auto i = 0; i < NRAND; i++)
        //nm.insert(new mc::BinaryNode(distribution(generator)));



    tree->echo();
    std::cout << "BinaryTree Test successful!\n\n";
    return EXIT_SUCCESS;
}
