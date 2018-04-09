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
#include "structures/BinaryTree.hpp"
#include "io/IOfactory.hpp"
#include "structures/Scheduler.hpp"

// NOTE: We initally assume all the Trees are the same
namespace monya {

    unsigned READ_SZ_BYTES = 4096; // TODO: Alter

    template <typename NodeType>
    class BinaryTreeProgram: public container::BinaryTree {
        private:
            typedef typename container::Scheduler sched_t;

        protected:
            short nnode_id; // NUMA node
            tree_t tree_id; // The ID of this tree
            std::string exmem_fn;
            io::IO::raw_ptr ioer;
            size_t nsamples; // Max # of samples from which the tree is built
            size_t nfeatures; // Number of features
            sched_t* scheduler;

        public:
            typedef std::shared_ptr<BinaryTreeProgram<NodeType> > ptr;

            void set_nnode_id(const short nnode_id) {
                this->nnode_id = nnode_id;
            }

            const short get_nnode_id() const {
                return nnode_id;
            }

            BinaryTreeProgram(Params& params, const tree_t tree_id) {
                this->tree_id = tree_id;
                this->exmem_fn = params.fn;
                ioer = IOfactory::create(params.iotype);
                ioer->set_fn(this->exmem_fn);
                ioer->set_dtype_size(8); // FIXME now!
                ioer->set_orientation(params.orientation);

                this->max_depth = params.max_depth;
                this->depth = 0;
                this->nsamples = params.nsamples;
                this->nfeatures = params.nfeatures;

                assert(params.fanout == 2);
                this->scheduler = new sched_t(params.fanout, params.ntree);
            }

            void set_root(NodeType* node) {
                assert(NULL != node);
                assert(NULL != ioer);

                node->set_ioer(ioer);
                BinaryTree::set_root(node);
            }

            NodeType* create_node() {
                return new NodeType;
            }

            sched_t* get_scheduler() {
                return scheduler;
            }

            void set_scheduler(sched_t* scheduler) {
                this->scheduler = scheduler;
            }

            static BinaryTreeProgram<NodeType>* create_raw() {
                return new BinaryTreeProgram<NodeType>();
            }

            static ptr create() {
                return ptr(new BinaryTreeProgram<NodeType>());
            }

            //static ptr cast2(typename container::BinaryTree::ptr rbptr) {
                //return std::static_pointer_cast<ptr>(rbptr);
            //}

            void set_ioer(io::IO::raw_ptr ioer) {
                this->ioer = ioer;
            }

            io::IO::raw_ptr get_ioer() {
                assert(NULL != this->ioer);
                return ioer;
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
            virtual void build() {
                assert(NULL != this->get_root());
                scheduler->schedule(this->get_root());
            }

            ~BinaryTreeProgram() {
                delete scheduler;
            }
    };
} // End monya

#endif
