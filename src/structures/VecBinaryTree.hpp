/*
 * Copyright 2018 Neurodata (https://neurodata.io)
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

#ifndef MONYA_VEC_BINARY_TREE_HPP__
#define MONYA_VEC_BINARY_TREE_HPP__

#include <memory>
#include "../common/types.hpp"
#include <cmath>

namespace monya { namespace container {

class BinaryNode;

// NOTE: No deletion!
class VecBinaryTree {
    private:
        const size_t get_depth(const size_t nelem) const {
            return std::floor(std::log2(nelem));
        }

        // Given a maximum depth we can compute how many indicies to have
        void prealloc(const depth_t max_depth) {
            nodes->resize(std::pow(2, max_depth) - 1);
        }

        const size_t size() const {
            return nodes->size();
        }

    size_t compute_idx(const size_t parent_idx, const bchild_t pos);
    protected:
        std::vector<BinaryNode>* nodes;

        size_t max_depth; // Maximum depth the tree can reach

    public:
        typedef std::shared_ptr<VecBinaryTree> ptr;

        VecBinaryTree() {
            // Guaranteed contiguous
            nodes = new std::vector<BinaryNode>();
        }

        static ptr create() {
            return ptr(new VecBinaryTree());
        }

        virtual BinaryNode* get_root() {
            return &((*nodes)[0]);
        }


        const size_t get_depth() const {
            return std::floor(std::log2(nodes->size()));
        }

        const bool empty() const {
            return nodes->empty();
        }

        const size_t get_parent_idx(const size_t node_idx) {
            return (node_idx - 1) / 2;
        }

        BinaryNode* get_at_idx(const size_t node_idx) {
            return &((*nodes)[node_idx]);
        }

        const size_t get_child_idx(const size_t parent_idx, const bchild_t pos);

        void set_root(BinaryNode node);
        virtual size_t get_nnodes();
        virtual void insert_at(BinaryNode new_node,
                BinaryNode* node, bchild_t pos);
        void insert_at(BinaryNode new_node,
                const size_t parent_idx, const bchild_t pos);
        virtual void insert(BinaryNode* node);
        BinaryNode* find(const BinaryNode* shell);
        void echo();
        ~VecBinaryTree();
};

} } // End monya::container

#endif
