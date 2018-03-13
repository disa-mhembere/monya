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

#include <cstdlib>
#include <random>

#include "../common/monya.hpp"
#include "../utils/time.hpp"

using namespace monya;

class kdnode: public container::RBNode<double> {
    public:

        // TODO: How to overcome this!? Not good ...
        kdnode* parent;
        kdnode* left;
        kdnode* right;

        void spawn() override {
            // TODO: Implement
        }

        void run() override {
            // TODO: Implement
            std::cout << "knode::run()";
        }
};

// How the program runs
class kdTreeProgram: public BinaryTreeProgram<kdnode> {
    private:
        // Dimension to split on
        size_t split_dim;
        std::default_random_engine generator;
        std::uniform_int_distribution<size_t> distribution;

        // All the trees in the forest (including this one!)
        std::vector<kdTreeProgram*> copse;

    public:

        // Can be used if we need no more constructors
        using BinaryTreeProgram<kdnode>::BinaryTreeProgram;

        kdTreeProgram(size_t nsamples, size_t nfeatures, size_t split_dim,
                short max_depth=-1, typename io::IO::raw_ptr ioer=NULL) :
            BinaryTreeProgram(nsamples, nfeatures, max_depth, ioer) {
                this->split_dim = split_dim;
                this->distribution =
                    std::uniform_int_distribution<size_t>(0, nfeatures);
            }

        void set_split_dim(const size_t split_dim) {
            this->split_dim = split_dim;
        }

        const size_t get_split_dim() const {
            return this->split_dim;
        }

        /**
         * Should be coordinated effort over all trees in the forest
         * @param randomize: Pick the split dimension at random
         */
        void pick_split_dim(bool randomize=false) {
            if (depth) { // We have more than the root
                if (randomize)
                    this->split_dim = distribution(generator);
                else
                    this->split_dim++;
            } else { // No nodes in tree
                this->split_dim = distribution(generator);
            }
        }

        void build() override {
            pick_split_dim(); // Choose split
        }
};

int main(int argc, char* argv[]) {
    // TODO: Read from argv[]
    size_t nsamples = 10;
    size_t nfeatures = 1;
    tree_t ntree = 1;
    unsigned nthread = 1;
    MAT_ORIENT mo = MAT_ORIENT::COL;

    Params params(nsamples, nfeatures,
            "/Research/monya/src/examples/data/ordered_tree_10.bin",
            IOTYPE::SYNC, ntree, nthread, mo);
    std::cout << params;

    utils::time t;
    ComputeEngine<kdTreeProgram>::ptr engine =
        ComputeEngine<kdTreeProgram>::create(params);
    // TODO: Add number of threads

    std::cout << "Engine created\n\n";
    //t.tic();
    engine->train();
    std::cout << "I'm well trained!\n";

    //std::cout << "Engine trained in " << t.toc() << " seconds\n";
    return EXIT_SUCCESS;
}
