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

#include "VecBinaryTree.hpp"
#include "BinaryNode.hpp"
#include "../../common/types.hpp"
#include <random>

#include "sucommon.hpp"

using namespace monya;
namespace mc = monya::container;
namespace mt = monya::test;

int main(int argc, char* argv[]) {
#ifdef VECNODES
    // Add some numbers in an ad hoc fashion
    constexpr unsigned NLEVELS = 3;
    constexpr unsigned FANOUT = 2;
    const unsigned NRAND = std::pow(FANOUT, NLEVELS) - 1;
    std::default_random_engine generator;
    std::uniform_real_distribution<data_t> distribution(0, 5);

    mc::VecBinaryTree::ptr tree = mc::VecBinaryTree::create();
    printf("Created tree!\n");

    // Deal with the root node special case separtely
    std::vector<mc::BinaryNode> init;
    for (unsigned i = 0; i < FANOUT; i++)
        init.push_back(mc::BinaryNode(distribution(generator)));
    printf("Inited!\n");

    monya::data_t mean = 0;
    for (auto node : init)
        mean +=  node.get_comparator();

    mc::BinaryNode root(mean/static_cast<monya::data_t>(init.size()));
    root.vecpos = 0;

    tree->set_root(root);
    std::cout << "Root is: \n";
    root.print();

    if (init.at(0) < init.at(1)) {
        tree->insert_at(init[0], tree->get_root(), bchild_t::LEFT);
        tree->insert_at(init[1], tree->get_root(), bchild_t::RIGHT);
    } else {
        tree->insert_at(init[1], tree->get_root(), bchild_t::LEFT);
        tree->insert_at(init[0], tree->get_root(), bchild_t::RIGHT);
    }
    printf("Root L and R set:\n");
    tree->get_at_idx(1)->print();
    tree->get_at_idx(2)->print();

    // Test arbitrary insertion
    for (size_t vecpos = 3; vecpos < NRAND; vecpos++) {
        mc::BinaryNode* parent = tree->get_at_idx(tree->get_parent_idx(vecpos));

        mc::BinaryNode node(mc::BinaryNode(distribution(generator)));
        if (vecpos % 2 ==0)
            tree->insert_at(node, parent, LEFT);
        else
            tree->insert_at(node, parent, RIGHT);
    }

    assert(tree->get_depth() == NLEVELS-1);

    std::cout << "\nEchoing tree:\n";
    tree->echo();

#endif
    std::cout << "VecBinaryTree Test successful!\n\n";
    return EXIT_SUCCESS;
}
