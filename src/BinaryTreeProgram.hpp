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
#include "structures/BinaryNode.hpp"
#include "io/IOfactory.hpp"
#include "structures/Scheduler.hpp"

// NOTE: We initally assume all the Trees are the same
namespace monya {
    class BinaryTreeProgram: public container::BinaryTree {
        private:

        protected:
            tree_t tree_id; // The ID of this tree
            short numa_id; // NUMA node
            std::string exmem_fn;
            io::IO::raw_ptr ioer;
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
                    const int _numa_id=0) :
                tree_id (_tree_id), numa_id(_numa_id) {

                exmem_fn = params.fn;
                // Configure ioer
                ioer = IOfactory::create(params.iotype);
                ioer->set_fn(exmem_fn);
                ioer->set_orientation(params.orientation);
                ioer->shape(dimpair(params.nsamples, params.nfeatures));

                if (params.iotype == io_t::MEM)
                    static_cast<io::MemoryIO*>(ioer)->from_file();

                // Configure Tree
                max_depth = params.max_depth;
                depth = 0;
                nsamples = params.nsamples;
                nfeatures = params.nfeatures;

                assert(params.fanout == 2);
                unsigned nworkers = std::max(1,
                        static_cast<int>(params.nthread/params.ntree));
#ifdef USE_NUMA
                scheduler = new container::Scheduler(params.fanout,
                        params.max_depth, tree_id, nworkers, numa_id);
#else
                scheduler = new container::Scheduler(params.fanout,
                        params.max_depth, tree_id, nworkers, 0;
#endif
            }

            void set_root(container::BinaryNode*& node) {
                assert(NULL != node);
                assert(NULL != ioer);

                node->set_ioer(ioer);
                BinaryTree::set_root(node);
            }

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

            void set_ioer(io::IO::raw_ptr ioer) {
                this->ioer = ioer;
            }

            io::IO::raw_ptr get_ioer() {
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

            void destroy() {
                if (ioer)
                    ioer->destroy();
            }

            // User implemented for training phase
            virtual void build() {
                assert(NULL != this->get_root());
                scheduler->schedule(this->get_root());

                bool one_spawned = false;
                while (true) {
                    depth_t procd_level = scheduler->get_current_level() - 1;
                    assert(procd_level <= max_depth);

                    std::vector<container::NodeView*> procd_nodes =
                        scheduler->get_nodes(procd_level);

                    // No skew for balanced tree
#pragma omp parallel for num_threads (get_nthread())
                    for (size_t i = 0; i < procd_nodes.size(); i++) {
                        container::BinaryNode* curr_node =
                            static_cast<container::BinaryNode*>(procd_nodes[i]);
                        // Termination conditions:
                        // 1. Max depth
                        // 2. Node has only one sample
                        if (curr_node->get_depth() < max_depth
                                && curr_node->get_data_index().size() > 1) {
                            curr_node->spawn();
                            if (!one_spawned) one_spawned = true;
                        }
                    }

                    // Terminal: No spawning or max depth reached
                    if (procd_level == max_depth || !one_spawned) break;
                    one_spawned = false; // Reset

                    for (size_t i = 0; i < procd_nodes.size(); i++) {
                        container::BinaryNode* curr_node =
                            static_cast<container::BinaryNode*>(procd_nodes[i]);
                            if (curr_node->left)
                                scheduler->schedule(curr_node->left);
                            if (curr_node->right)
                                scheduler->schedule(curr_node->right);
                    }
                }
            }

            const unsigned get_nthread() { return scheduler->get_nthread(); }

            ~BinaryTreeProgram() {
                delete scheduler;
            }
    };
} // End monya
#endif
