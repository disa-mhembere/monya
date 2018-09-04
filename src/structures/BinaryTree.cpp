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
#include "../common/exception.hpp"
#include "BinaryNode.hpp"
#include "Query.hpp"

namespace monya { namespace container {

    void BinaryTree::echo(BinaryNode* node, int tabs) {
        if (!node) {
            return;
        }

        if (node->left)
            echo(&node->children[0], tabs + 1);

        for (int i = 0; i < tabs; ++i) {
            std::cout << "\t\t";
        }
        std::cout << node->get_comparator() << std::endl;

        if (node->right) {
            if (node->left)
                echo(&node->children[1], tabs + 1);
            else
                echo(&node->children[0], tabs + 1);
        }
    }

    void BinaryTree::delete_node(BinaryNode* node) {
        if (!node) {
            return;
        }

        if (node->left) {
            delete_node(&node->children[0]);
        }

        if (node->right) {
            if (!node->left)
                delete_node(&node->children[0]);
            else
                delete_node(&node->children[1]);
        }
        delete node;
    }

    void BinaryTree::set_root(BinaryNode*& node) {
        assert(NULL != node);
        root = node;
    }

    size_t BinaryTree::get_nnodes(BinaryNode* node, size_t& nnodes) {
        if (!node) {
            return nnodes;
        }

        nnodes++;
        if (node->left)
            get_nnodes(&node->children[0], nnodes);

        if (node->right) {
            if (node->left)
                get_nnodes(&node->children[1], nnodes);
            else
                get_nnodes(&node->children[0], nnodes);
        }

        return nnodes;
    }

    void BinaryTree::echo() {
        echo(root, 0);
    }

    BinaryTree::~BinaryTree() {
        delete_node(root);
    }
} } // End monya::container
