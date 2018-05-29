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

#ifndef MONYA_BINARY_TREE_HPP__
#define MONYA_BINARY_TREE_HPP__

#include <memory>
#include "../common/types.hpp"
#include "../common/exception.hpp"

namespace monya { namespace container {

class BinaryNode;
class Query;

// NOTE: No deletion!
class BinaryTree {
    protected:
        BinaryNode* root;
        size_t depth;
        size_t max_depth; // Maximum depth the tree can reach

        void echo(BinaryNode* node, int tabs);
        void delete_node(BinaryNode* node); // Used for cleanup only

    public:
        typedef std::shared_ptr<BinaryTree> ptr;

        BinaryTree(): root(NULL) {
        }

        static ptr create() {
            return ptr(new BinaryTree());
        }

        virtual NNvector find_neighbors(Query* q) {
            throw not_implemented_exception();
        }

        virtual std::vector<BinaryNode*> find_neighbor_nodes(Query* q) {
            throw not_implemented_exception();
        }


        virtual BinaryNode* get_root() {
            return root;
        }

        void set_depth(const size_t depth) {
            this->depth = depth;
        }

        const size_t get_depth() const {
            return depth;
        }

        const bool empty() const {
            return NULL == root;
        }

        virtual void set_root(BinaryNode*& node);
        virtual size_t get_nnodes(BinaryNode* node, size_t& nnodes);
        virtual void insert_at(BinaryNode*& new_node,
                BinaryNode* node, bchild_t pos);
        virtual void insert(BinaryNode*& node);
        BinaryNode* find(const BinaryNode* shell);
        void echo();
        ~BinaryTree();
};

} } // End monya::container

#endif
