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

    mc::BinaryNode* root = new mc::BinaryNode(10);
#if 0
    std::cout << "Print test: " << *root << std::endl;
#endif
    root->print();

    mc::BinaryNode* lnode = new mc::BinaryNode(4);
    lnode->children = new mc::BinaryNode[2];
    lnode->left = lnode->right = true;
#if 0
    std::cout << *lnode << std::endl;
#endif
    root->children = new mc::BinaryNode[2];
    root->left = root-> right = true;
    root->children[0] = *lnode;

    mc::BinaryNode* rnode = new mc::BinaryNode(14);
#if 0
    std::cout << *rnode << std::endl;
#endif
    root->children[1] = *rnode;

    //root->lnode->rnode = rnode;
    //root->rnode->lnode = lnode;

    assert(root->children[0] == *lnode);
    assert(root->children[1] == *rnode);
    //assert(*root->lnode->rnode == *rnode);
    //assert(*root->rnode->lnode == *lnode);

    delete(lnode);
    delete(rnode);
    delete(root);

    std::cout << "BinaryNode Test successful!\n\n";
    return EXIT_SUCCESS;
}
