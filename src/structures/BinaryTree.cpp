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

        echo(node->left, tabs + 1);

        for (int i = 0; i < tabs; ++i) {
            std::cout << "\t\t";
        }
        std::cout << node->get_comparator() << std::endl;

        echo(node->right, tabs + 1);
    }

    void BinaryTree::delete_node(BinaryNode* node) {
        if (!node) {
            return;
        }

        if (node->left) {
            delete_node(node->left);
        }

        if (node->right) {
            delete_node(node->right);
        }

        delete node;
    }

    void BinaryTree::set_root(BinaryNode*& node) {
        assert(NULL != node);
        insert(node);
    }

    size_t BinaryTree::get_nnodes(BinaryNode* node, size_t& nnodes) {
        if (!node) {
            return nnodes;
        }

        nnodes++;
        get_nnodes(node->left, nnodes);
        get_nnodes(node->right, nnodes);
        return nnodes;
    }

    void BinaryTree::insert_at(BinaryNode*& new_node,
            BinaryNode* node, bchild_t pos) {
        if (pos == bchild_t::LEFT && NULL != node->left) {
            throw std::runtime_error("BinaryTree::insert_at: "
                    "Left child already populated");
        } else if (pos == bchild_t::RIGHT && NULL != node->right) {
            throw std::runtime_error("BinaryTree::insert_at: "
                    "Right child already populated");
        }

        BinaryNode* parent = node;

        // If the parent is NULL then the tree is empty
        if (!parent) {
            new_node = root = node;
            new_node->parent = new_node->left = new_node->right = NULL;
            depth = 1; // Data race ok
        } else {
            new_node->parent = parent;
            new_node->left = new_node->right = NULL;

            if (pos == bchild_t::LEFT)
                parent->left = new_node;
            else
                parent->right = new_node;
        }
    }

    void BinaryTree::insert(BinaryNode*& node) {
        BinaryNode* tmp; // A tmp node used to find the parent
        BinaryNode* parent; // Will hold the parent of node being inserted
        BinaryNode* new_node = NULL; // The new node being inserted

        parent = NULL;
        tmp = root;
        // Traverse the tree to find the parent of the node to be inserted
        while (tmp) {
            parent = tmp;
            if (*node < *tmp) {
                tmp = tmp->left;
            } else {
                tmp = tmp->right;
            }
        }

        // If the parent is NULL then the tree is empty
        if (!parent) {
            new_node = root = node;
            new_node->parent = new_node->left = new_node->right = NULL;
        } else {
            new_node = node;
            new_node->parent = parent;
            new_node->left = new_node->right = NULL;

            if (*(new_node) < *(parent)) {
                parent->left = new_node;
            } else {
                parent->right = new_node;
            }
        }
    }

    BinaryNode* BinaryTree::find(const BinaryNode* shell) {
        BinaryNode *node = root;
        while (node) {
            if (*node > *shell) {

                node = node->left;
            } else if (*node < *shell) {

                node = node->right;
            } else {
                return node;
            }
        }

        throw std::runtime_error("node not found");
    }

    void BinaryTree::echo() {
        echo(root, 0);
    }

    BinaryTree::~BinaryTree() {
        delete_node(root);
    }
} } // End monya::container
