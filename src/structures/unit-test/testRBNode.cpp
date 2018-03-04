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

    mc::RBNode<double>* root = new mc::RBNode<double>(10);
    root->color = 0;
    std::cout << "Print test: " << *root << std::endl;
    root->print();

    mc::RBNode<double>* left = new mc::RBNode<double>(4);
    left->color = 1;
    std::cout << *left << std::endl;
    root->left = left;

    mc::RBNode<double>* right = new mc::RBNode<double>(14);
    right->color = 1;
    std::cout << *right << std::endl;
    root->right = right;

    root->left->right = right;
    root->right->left = left;

    assert(*(root->right) == *right);
    assert(*(root->left) == *left);
    assert(*(root->left->right) == *right);
    assert(*(root->right->left) == *left);
    assert(root->right->color == right->color);
    assert(root->left->color == left->color);

    mc::RBNode<double>* t = new mc::RBNode<double>();
    t->color = 0;
    assert(!t->color);

    delete(left);
    delete(right);
    delete(root);

    std::cout << "RBNode Test successful!\n\n";
    return EXIT_SUCCESS;
}
