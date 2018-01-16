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

#ifndef TREE_PROGRAM_HPP__
#define TREE_PROGRAM_HPP__

#include "common/types.hpp"
#include "structures/RBTree.hpp"

// NOTE: We initally assume all the Trees are the same
namespace monya {

    unsigned READ_SZ_BYTES = 4096; // TODO: Alter

    template <typename NodeType>
    class TreeProgram: container::RBTree<NodeType> {
        private:
            short nnode_id; // NUMA node
            tree_t tree_id; // The ID of this tree
            std::string exmem_fn;

        public:
            TreeProgram() {
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
    };
} // End monya

#endif
