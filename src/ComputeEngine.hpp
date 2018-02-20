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

namespace monya {
    class Params;

    template <typename TreeProgramType>
    class ComputeEngine {

        private:
            std::vector<TreeProgramType*> forest; // For when there are more
            tree_t ntree;
            Params params;

            ComputeEngine(const tree_t ntree=1) {
                this->ntree = ntree;
                for (tree_t tid = 0; tid < ntree; tid++) {
                    forest.push_back(new TreeProgramType);
                }
            }

            ComputeEngine(Params& params, const tree_t ntree):
                ComputeEngine(ntree) {
                this->params = params;
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

            static ptr create() {
                return ptr(new ComputeEngine<TreeProgramType>());
            }

            static ptr create(std::vector<TreeProgramType*>& forest) {
                return ptr(new ComputeEngine(forest));
            }

            static ptr create(Params& params, const tree_t ntree) {
                return ptr(new ComputeEngine<TreeProgramType>(params, ntree));
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

            void add_tree(TreeProgramType* tree) {
                this->forest.push_back(tree);
            }

            void train() {
                for (size_t tree_id = 0; tree_id < ntree; tree_id++) {
                    forest[tree_id]->build();
                }
            }

            void predict() {

            }

            TreeProgramType* get_tree(const tree_t id) {
                return forest[id];
            }

            ~ComputeEngine() {
                for (size_t tree_id = 0; tree_id < ntree; tree_id++) {
                    forest[tree_id]->destroy();
                }
            }
    };
} // End monya

#endif
