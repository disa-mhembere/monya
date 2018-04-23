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

#include "RBNode.hpp"

namespace mc = monya::container;

int main(int argc, char* argv[]) {

    mc::BinaryNode* root = new mc::RBNode(10);
    mc::RBNode::cast2(root)->color = 0;
#if 0
    std::cout << "Print test: " << *root << std::endl;
#endif
    root->print();

    mc::BinaryNode* left = new mc::RBNode(4);
    mc::RBNode::cast2(left)->color = 1;
#if 0
    std::cout << *left << std::endl;
#endif
    root->left = left;

    mc::BinaryNode* right = new mc::RBNode(14);
    mc::RBNode::cast2(right)->color = 1;
#if 0
    std::cout << *right << std::endl;
#endif
    root->right = right;

    root->left->right = right;
    root->right->left = left;

    assert(*(root->right) == *right);
    assert(*(root->left) == *left);
    assert(*(root->left->right) == *right);
    assert(*(root->right->left) == *left);
    assert(mc::RBNode::cast2(root->right)->color
            == mc::RBNode::cast2(right)->color);

    assert(mc::RBNode::cast2(root->left)->color
            == mc::RBNode::cast2(left)->color);

    mc::BinaryNode* t = new mc::RBNode;
    mc::RBNode::cast2(t)->color = 0;
    assert(!mc::RBNode::cast2(t)->color);

    delete(left);
    delete(right);
    delete(root);

    std::cout << "RBNode Test successful!\n\n";
    return EXIT_SUCCESS;
}
