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
};

// How the program runs
class kdTreeProgram: public BinaryTreeProgram<kdnode> {
    private:
        size_t split_dim;
        std::default_random_engine generator;
        std::uniform_int_distribution<size_t> distribution;

    public:
        kdTreeProgram(): BinaryTreeProgram<kdnode>() {
            split_dim = 0;
            //generator.a = 0;
        }

        kdTreeProgram(size_t max_depth): BinaryTreeProgram<kdnode>() {
            this->split_dim = split_dim;
        }

        void set_split_dim(const size_t split_dim) {
            this->split_dim = split_dim;
        }

        /**
         * Should be coordinated effort over all trees in the forest
         * @param randomize: Pick the split dimension at random
         */
        void pick_split_dim(bool randomize=false) {
            if (depth) {
                ; // TODO: coordinate
            } else {
                if (randomize)
                    ; // TODO
                else
                    ; // TODO
            }
        }

        void build() override {
            // TODO: Implement
        }
};

int main(int argc, char* argv[]) {
    Params params("/mnt/nfs/disa/monya/src/structures/unit-test/"
            "data/ordered_tree_10.bin", IOTYPE::SYNC, 1);

    //utils::time t;
    ComputeEngine<kdTreeProgram>::ptr engine =
        ComputeEngine<kdTreeProgram>::create(params, 2);
    //t.tic();
    engine->train();
    std::cout << "I'm well trained!\n";

    //std::cout << "Engine trained in " << t.toc() << " seconds\n";
    return EXIT_SUCCESS;
}
