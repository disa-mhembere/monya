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

#include "BinaryNode.hpp"

namespace mc = monya::container;

int main(int argc, char* argv[]) {

    mc::BinaryNode<double>* root = new mc::BinaryNode<double>(10);
    std::cout << "Print test: " << *root << std::endl;
    root->print();

    mc::BinaryNode<double>* left = new mc::BinaryNode<double>(4);
    std::cout << *left << std::endl;
    root->left = left;

    mc::BinaryNode<double>* right = new mc::BinaryNode<double>(14);
    std::cout << *right << std::endl;
    root->right = right;

    root->left->right = right;
    root->right->left = left;

    assert(*(root->right) == *right);
    assert(*(root->left) == *left);
    assert(*(root->left->right) == *right);
    assert(*(root->right->left) == *left);

    delete(left);
    delete(right);
    delete(root);

    std::cout << "BinaryNode Test successful!\n\n";
    return EXIT_SUCCESS;
}
