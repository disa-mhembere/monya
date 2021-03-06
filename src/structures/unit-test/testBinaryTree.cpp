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

using namespace monya;

void test_manual_construction() {
    container::BinaryTree::ptr tree = container::BinaryTree::create();

    container::BinaryNode* root = new container::BinaryNode(5.0);
    tree->set_root(root);

    container::BinaryNode* l = new container::BinaryNode(2.0);
    l->parent = root;
    root->left = l;
    container::BinaryNode* r = new container::BinaryNode(8.0);
    r->parent = root;
    root->right = r;

    l->left = l->right = r->left = r->right = NULL;

    container::BinaryNode* ll = new container::BinaryNode(1.0);
    ll->parent = l;
    l->left = ll;

    container::BinaryNode* lr = new container::BinaryNode(4.0);
    lr->parent = l;
    l->right = lr;

    container::BinaryNode* rl = new container::BinaryNode(6.0);
    rl->parent = r;
    r->left = rl;

    container::BinaryNode* rr = new container::BinaryNode(10.0);
    rr->parent = r;
    r->right = rr;

    ll->left = ll->right = lr->left = lr->right
        = rl->left = rl->right = rr->right = rr->left = NULL;

    tree->echo();
}

int main(int argc, char* argv[]) {
    test_manual_construction();

    ////////////////////////////////////////////////////////////////////////////

    std::vector<data_t> members {0.10, 0.8, 0.14, 0.6};

    container::BinaryTree::ptr tree = container::BinaryTree::create();

    for (std::vector<data_t>::iterator it = members.begin();
            it != members.end(); ++it) {
        container::BinaryNode* node = new container::BinaryNode((data_t)*it);
        tree->insert(node);
    }

#ifdef MONYA_VERBOSE
    std::cout << "Printing tree: \n";
    tree->echo();
#endif

    for (auto i : members) {
        // Asserts on failure to find
        container::BinaryNode* q = new container::BinaryNode(i);
        container::BinaryNode* a = tree->find(q);
        assert(*q == *a);
        delete(q);
    }

    // Add some numbers in an ad hoc fashion
    constexpr unsigned NLEVELS = 3;
    constexpr unsigned FANOUT = 2;
    constexpr unsigned NRAND = std::pow(FANOUT, NLEVELS) - 1;
    std::default_random_engine generator;
    std::uniform_real_distribution<data_t> distribution(0, 5);

    container::BinaryTree::ptr tree2 = container::BinaryTree::create();

    test::NodeMapper nm;

    // Deal with the root node special case separtely
    std::vector<container::BinaryNode*> init;
    for (unsigned i = 0; i < FANOUT; i++)
        init.push_back(new container::BinaryNode(distribution(generator)));

    monya::data_t mean = 0;
    for (auto node : init)
        mean +=  node->get_comparator();

    nm.insert(new container::BinaryNode(mean/static_cast<monya::data_t>(init.size())));
    for (auto node : init)
        mean +=  node->get_comparator();

    // Test arbitrary insertion
    for (unsigned i = 0; i < NRAND-(FANOUT+1); i++) {
        nm.insert(new container::BinaryNode(distribution(generator)));
    }

    std::cout << "nm:\n";
    nm.print();
    nm.populate_tree(tree2);
    std::cout << "tree2:\n";
    tree2->echo();

#if 0
    // Now test nodes are where they're supposed to be
    for (size_t i = 0; i < nm.size(); i++) {
        auto nodes = nm.map[i];

        if (i == 0) {
            std::cout << "Testing root\n";
            assert(NULL == nodes[0]->parent);
            assert(nodes[0] == tree2->get_root());
        } else {

            for (size_t ii = 0; ii < nodes.size(); ii++) {
                // Already an assert on find failure
                container::BinaryNode* node = nodes[ii];
                std::cout << "Looking for "; node->print(); std::cout << "\n";

                auto ret = tree2->find(node); // Find this node in the tree2
                std::cout << "found!\n";
                auto parent = ret->parent;

                // Parent test
                assert(parent == node->parent);
                assert(parent == (nm.map[i-1][ii/2]));

                // Sibling test since binary this can work
                if (ii % 2 == 0) {
                    auto sibling = nodes[ii+1];
                    if (*node < *sibling)
                        assert(parent->right == sibling);
                    else
                        assert(parent->left == sibling);
                }

            }
        }
    }
#endif

#ifdef MONYA_VERBOSE
    std::cout << "Printing tree2: \n";
    tree2->echo();
#endif

    std::cout << "BinaryTree Test successful!\n\n";
    return EXIT_SUCCESS;
}
