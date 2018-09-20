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

#include "../common/types.hpp"
#include "BinaryTree.hpp"
#include "BinaryNode.hpp"
#include <omp.h>

// NOTE: We initally assume all the Trees are the same
namespace monya {
    namespace container {
        class Scheduler;
    }

    namespace io {
        class IO;
    }

    class BinaryTreeProgram: public container::BinaryTree {
        private:

        protected:
            tree_t tree_id; // The ID of this tree
            short numa_id; // NUMA node
            std::string exmem_fn;
            io::IO* ioer;
            size_t nsamples; // Max # of samples from which the tree is built
            size_t nfeatures; // Number of features
            container::Scheduler* scheduler;

        public:
            typedef std::shared_ptr<BinaryTreeProgram> ptr;

            // Use BinaryTree ctor
            BinaryTreeProgram() : container::BinaryTree() {
            }

            void set_numa_id(const short numa_id) {
                this->numa_id = numa_id;
            }

            const short get_numa_id() const {
                return numa_id;
            }

            BinaryTreeProgram(Params& params, const tree_t _tree_id,
                    const int _numa_id=0);

            void set_root(container::BinaryNode*& node);

            container::Scheduler* get_scheduler() {
                return scheduler;
            }

            void set_scheduler(container::Scheduler* scheduler) {
                this->scheduler = scheduler;
            }

            static BinaryTreeProgram* create_raw() {
                return new BinaryTreeProgram();
            }

            static ptr create() {
                return ptr(new BinaryTreeProgram);
            }

            void set_ioer(io::IO* ioer) {
                this->ioer = ioer;
            }

            io::IO* get_ioer() {
                assert(NULL != this->ioer);
                return ioer;
            }

            void set_id(const tree_t tid) {
                this->tree_id = tid;
            }

            const tree_t get_id() const {
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

            virtual void destroy();

            // User implemented for training phase
            virtual void build();
            const unsigned get_nthread();

            ~BinaryTreeProgram();
    };
} // End monya
#endif
