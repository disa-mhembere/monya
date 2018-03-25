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
constexpr short MAX_DEPTH = 3;

class kdnode: public container::RBNode {
    private:
        size_t split_dim;

    public:
        // Define lineage
        kdnode* parent;
        kdnode* left;
        kdnode* right;

        void spawn(std::vector<sample_id_t>& idxs,
                std::vector<offset_t>& offsets) override {
            std::cout << "TODO\n";
        }

        void set_split_dim(size_t split_dim) {
            this->split_dim = split_dim;
        }

        const size_t get_split_dim() const {
            return split_dim;
        }

        // This is run first
        void prep() override {
            get_data(); // Put data into mem
        }

        // This is run next
        void run() override {
            std::cout << "kdnode at depth: " << depth << " run()\n";
            if (depth < 3)
                sort_data_index(true); // Paralleize the sort
            else
                sort_data_index(false);

            std::vector<sample_id_t> idxs;
            data_index.get_indexes(idxs);

            // Integer division
            std::vector<offset_t> offsets = { data_index.size() / 2 };
            assert(idxs.size() == data_index.size());

            // TODO: How to handle max depth handled elsewhere ??
            if (depth < MAX_DEPTH) {
                spawn(idxs, offsets);
            }
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

    params.print();

    //utils::time t;
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
                root->set_split_dim(split_dim); // Which dim to split on
                root->set_index(0, params.nsamples); // Which samples it owns
                root->set_scheduler((*it)->get_scheduler());

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
