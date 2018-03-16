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
#include <set>

#include "../common/monya.hpp"
#include "../utils/time.hpp"

using namespace monya;

class kdnode: public container::RBNode<double> {
    private:
        size_t split_dim;

    public:
        // TODO: How to overcome this!? Not good ...
        kdnode* parent;
        kdnode* left;
        kdnode* right;

        void spawn() override {
            // TODO: Implement
        }

        void set_split_dim(size_t split_dim) {
            this->split_dim = split_dim;
        }

        const size_t get_split_dim() const {
            return split_dim;
        }

        void init(Params& params) override {
            if (is_root()) {
                auto nsamples = params.nsamples;
                set_index(0, nsamples);
            } else {
            }
        }

        void run() override {
            // TODO: Implement
            std::cout << "PLACEHOLDER: knode::run()\n";
        }

        const void print() const override {
            std::cout << "\n\nHA!\n\n";
        }
};

// How the program runs
class kdTreeProgram: public BinaryTreeProgram<kdnode> {
    private:
        // All the trees in the forest (including this one!)
        std::vector<kdTreeProgram*> copse;

    public:
        // Can be used if we need no more constructors
        using BinaryTreeProgram<kdnode>::BinaryTreeProgram;
};

class RandomSplit {
    private:
        std::default_random_engine generator;
        std::uniform_int_distribution<size_t> distribution;
    public:
        RandomSplit(size_t nfeatures) {
            distribution = std::uniform_int_distribution<size_t>(0, nfeatures);
        }

        size_t generate() {
            return distribution(generator);
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
    std::cout << "Engine created ...\n";

    // Create the root with no duplicates for split dimension
    // TODO: Create root/node initializer that is passed to node
    RandomSplit rs(params.nfeatures);
    std::set<size_t> splits;
    for (auto it = engine->forest_begin(); it != engine->forest_end(); ++it) {
        while (true) {
            size_t split_dim = rs.generate();
            auto sp = splits.find(split_dim); // Pick the split dim
            if (sp == splits.end()) {
                std::cout << "Choosing split: " << split_dim << std::endl;

                kdnode* root = new kdnode;
                root->set_split_dim(split_dim);
                (*it)->set_root(root);

                splits.insert(split_dim); // Keep track of used split dims
                break;
            }
        }
    }
    std::cout << "Roots initialized ...\n";

    //t.tic();
    engine->train();
    std::cout << "I'm well trained!\n";

    //std::cout << "Engine trained in " << t.toc() << " seconds\n";
    return EXIT_SUCCESS;
}
