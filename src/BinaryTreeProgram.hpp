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

#ifndef MONYA_BINARY_TREE_PROGRAM_HPP__
#define MONYA_BINARY_TREE_PROGRAM_HPP__

#include "common/types.hpp"
#include "structures/RBTree.hpp"
#include "io/IO.hpp"

// NOTE: We initally assume all the Trees are the same
namespace monya {

    unsigned READ_SZ_BYTES = 4096; // TODO: Alter

    template <typename NodeType>
    class BinaryTreeProgram: container::RBTree<NodeType> {
        protected:
            short nnode_id; // NUMA node
            tree_t tree_id; // The ID of this tree
            std::string exmem_fn;
            io::IO::raw_ptr ioer;
            short max_depth; // Maximum depth the tree can reach
            short depth; // The current depth of this tree
            size_t nsamples; // Max # of samples from which the tree is built
            size_t nfeatures; // Number of features

        public:
            typedef std::shared_ptr<BinaryTreeProgram<NodeType> > ptr;

            BinaryTreeProgram(size_t nsamples, size_t nfeatures,
                    short max_depth=-1, typename io::IO::raw_ptr ioer=NULL) {
                this->nsamples = nsamples;
                this->nfeatures = nfeatures;
                this->ioer = ioer;
                this->max_depth = max_depth;
            }

            static BinaryTreeProgram<NodeType>* create_raw() {
                return new BinaryTreeProgram<NodeType>();
            }

            static ptr create() {
                return ptr(new BinaryTreeProgram<NodeType>());
            }

            static ptr cast2(typename container::RBTree<NodeType>::ptr rbptr) {
                return std::static_pointer_cast<ptr>(rbptr);
            }

            void set_ioer(io::IO::raw_ptr ioer) {
                this->ioer = ioer;
            }

            void set_tree_id(const tree_t tid) {
                this->tree_id = tid;
            }

            const tree_t get_tree_id() const {
                return tree_id;
            }

            const std::string get_exmem_fn() const {
                return this->exmem_fn;
            }

            void set_exmem_fn(const std::string exmem_fn) {
                this->exmem_fn = exmem_fn;
            }

            void descend() {
                depth++;
            }

            void destroy() {
                if (ioer)
                    ioer->destroy();
            }

            // User implemented for training phase
            virtual void build() = 0;
    };
} // End monya

#endif