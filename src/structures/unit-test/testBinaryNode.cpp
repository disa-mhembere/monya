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

    mc::NodeView<double>* root = new mc::BinaryNode<double>(10);
    std::cout << "Print test: " << root << std::endl;
    root->print();

#if 0
    mc::NodeView<double>* left = new mc::BinaryNode<double>(4);
    static_cast<mc::BinaryNode<double>*>(left)->print();
    static_cast<mc::BinaryNode<double>*>(root)->left(left);

    mc::NodeView<double>* right = new mc::BinaryNode<double>(14);
    right->print();
    static_cast<mc::BinaryNode<double>*>(root)->right(right);

    delete(left);
    delete(right);
#endif
    delete(root);

    return EXIT_SUCCESS;
}
