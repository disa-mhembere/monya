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
#include "BinaryNode.hpp"

namespace mc = monya::container;

int main(int argc, char* argv[]) {

    mc::NodeView<double>* root = new mc::BinaryNode<double>(10);

    mc::NodeView<double>* left = new mc::BinaryNode<double>(4);
    mc::BinaryNode<double>::cast2(root)->left(left);

    mc::NodeView<double>* right = new mc::BinaryNode<double>(14);
    mc::BinaryNode<double>::cast2(root)->right(right);

    mc::RBTree<mc::BinaryNode<double>*>::ptr tree =
        mc::RBTree<mc::BinaryNode<double>* >::create();
    tree->insert(mc::BinaryNode<double>::cast2(root));

    tree->echo();
    return EXIT_SUCCESS;
}
