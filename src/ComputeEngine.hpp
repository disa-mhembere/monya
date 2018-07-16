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

#ifndef MONYA_COMPUTE_ENGINE_HPP__
#define MONYA_COMPUTE_ENGINE_HPP__

#include <memory>
#include <vector>
#include "common/types.hpp"
#include "common/exception.hpp"
#include "structures/Query.hpp"

namespace monya {
    template <typename TreeProgramType>
    class ComputeEngine {

        private:
            std::vector<TreeProgramType*> forest; // For when there are more
            Params params;

            ComputeEngine(Params& params) {
                this->params = params;

                for (tree_t tree_id = 0; tree_id < params.ntree; tree_id++)
                    forest.push_back(new TreeProgramType(params, tree_id));
            }

            ComputeEngine(std::vector<TreeProgramType*>& forest) {
                this->forest = forest;
            }

            ComputeEngine(Params& params,
                    std::vector<TreeProgramType*>& forest) {
                this->params = params;
                this->forest = forest;
            }

        public:
            typedef std::shared_ptr<ComputeEngine<TreeProgramType> > ptr;
            typedef typename std::vector<TreeProgramType*>::iterator forest_iterator;

            static ptr create() {
                return ptr(new ComputeEngine<TreeProgramType>());
            }

            static ptr create(std::vector<TreeProgramType*>& forest) {
                return ptr(new ComputeEngine(forest));
            }

            static ptr create(Params& params) {
                return ptr(new ComputeEngine<TreeProgramType>(params));
            }

            static ptr create(Params& params,
                    std::vector<TreeProgramType*>& forest) {
                return ptr(new ComputeEngine<TreeProgramType>(params, forest));
            }

            void set_params(const Params& params) {
                this->params = params;
            }

            const Params& get_params() {
                return this->params;
            }

            std::vector<TreeProgramType*>& get_forest() {
                return this->forest;
            }

            forest_iterator forest_begin() {
                return this->forest.begin();
            }

            forest_iterator forest_end() {
                return this->forest.end();
            }

            void add_tree(TreeProgramType* tree) {
                this->forest.push_back(tree);
            }

            void train() {
                assert(forest.size() == params.ntree);

                for (auto it = forest.begin(); it != forest.end(); ++it)
                    (*it)->build();
            }

            void query(container::Query* pq) {
                for (auto tree : forest) {
                    pq->run(tree);
                }
            }

            void predict() {
                throw not_implemented_exception();
            }

            TreeProgramType* get_tree(const tree_t id) {
                return forest[id];
            }

            ~ComputeEngine() {
                assert(forest.size() == params.ntree);
                for (auto it = forest.begin(); it != forest.end(); ++it)
                    (*it)->destroy();
            }
    };
} // End monya

#endif
