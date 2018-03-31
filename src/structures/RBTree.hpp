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

#ifndef MONYA_RB_TREE_HPP__
#define MONYA_RB_TREE_HPP__

#include <memory>
#include "../common/types.hpp"

namespace monya { namespace container {

class RBNode;
//class Scheduler;

class RBTree {
    private:
        RBNode* root;
        size_t depth;
        //Scheduler scheduler;

        void rotate_left(RBNode* x);
        void rotate_right(RBNode* y);
        void transplant(RBNode* dest, RBNode* src);

        RBNode* minimum(RBNode* tree);
        void echo(RBNode* node, int tabs);
        void delete_node(RBNode* node);

    public:
        typedef std::shared_ptr<RBTree> ptr;

        RBTree(): root(NULL) {
        }

        static ptr create() {
            return ptr(new RBTree());
        }

        virtual RBNode* get_root() {
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

        void set_root(RBNode* node);
        size_t get_nnodes(RBNode* node, size_t& nnodes);
        void insert_at(RBNode* new_node, RBNode* node, bchild_t pos);
        void insert(RBNode* node);
        void balance(RBNode* parent, RBNode* node, RBNode* new_node);
        RBNode* find(const RBNode* shell);
        void _delete(const RBNode& shell);
        void echo();
        ~RBTree();
};
} } // End monya::container

#endif
