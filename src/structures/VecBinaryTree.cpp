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
#include "../common/exception.hpp"
#include "BinaryNode.hpp"

namespace monya { namespace container {

    void VecBinaryTree::echo() {
        if (empty()) {
            return;
        }

        size_t print_depth = 0;

        for (size_t i = 0; i < nodes->size(); i++) {
            if (get_depth(i+1) > print_depth) {
                std::cout << "\n";
                print_depth++;
            }
            std::cout << (*nodes)[i].get_comparator() << " ";
        }
        std::cout << "\n";
    }

    // Get the index of a child given the index of the parent
    size_t VecBinaryTree::compute_idx(const size_t parent_idx,
            const bchild_t pos) {

        switch (pos) {
            case bchild_t::LEFT:
                return (2 * parent_idx) + 1;
            case bchild_t::RIGHT:
                return (2 * parent_idx) + 2;
            default:
                throw parameter_exception("Unknown child type for binary "
                        "tree", __FILE__, __LINE__);
        }
    }

    // TODO: See if we can eliminate copy
    void VecBinaryTree::set_root(BinaryNode node) {
        if (empty())
            nodes->push_back(node);
        else
            (*nodes)[0] = node;
    }

    size_t VecBinaryTree::get_nnodes() {
        return nodes->size();
    }

    const size_t VecBinaryTree::get_child_idx(
            const size_t parent_idx, const bchild_t pos) {
        if (pos == bchild_t::LEFT)
            return (2 * parent_idx + 1);
        else if (pos == bchild_t::RIGHT)
            return (2 * parent_idx + 2);
        else
            throw parameter_exception("Unknown bchild_t type",
                    __FILE__, __LINE__);
    }

    // TODO: Check if we can avoid copy here
    void VecBinaryTree::insert_at(BinaryNode new_node,
            BinaryNode* node, bchild_t pos) {
#ifdef VECNODES
        // Compute position
        size_t child_pos = get_child_idx(node->vecpos, pos);
        new_node.vecpos = child_pos;


        // TODO: Inherit other metadata
        // FIXME: Preallocate when threading only works for 1 thread
        nodes->push_back(new_node);
        //if (nodes->size() < child_pos + 1) {
            //nodes->resize(child_pos + 1);
            //(*nodes)[child_pos] = new_node;
            //}
#endif
    }

    // TODO: Check if we can avoid copy here
    void VecBinaryTree::insert_at(BinaryNode new_node,
            const size_t parent_idx, const bchild_t pos) {
        // Compute position
#ifdef VECNODES
        size_t child_pos = get_child_idx(parent_idx, pos);
        new_node.vecpos = child_pos;


        // TODO: Inherit other metadata
        // FIXME: Preallocate when threading only works for 1 thread
        nodes->push_back(new_node);
#endif
    }

    void VecBinaryTree::insert(BinaryNode* node) {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    BinaryNode* VecBinaryTree::find(const BinaryNode* shell) {

        //size_t idx = 0;

        //BinaryNode node = root;
        //while (node) {
            //if (*node > *shell) {

                //node = node->left;
            //} else if (*node < *shell) {

                //node = node->right;
            //} else {
                //return node;
            //}
        //}

        throw std::runtime_error("node not found");
    }

    VecBinaryTree::~VecBinaryTree() {
        delete nodes;
    }
} } // End monya::container
