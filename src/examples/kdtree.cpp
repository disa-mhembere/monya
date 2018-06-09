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
#include "../io/IO.hpp"
#include "../structures/SampleVector.hpp"

using namespace monya;

class kdnode: public container::BinaryNode {
    private:
        size_t split_dim;

    public:
        // Inherit constructors
        using container::BinaryNode::BinaryNode;

        kdnode() {
            parent = left = right = NULL;
        }

        static kdnode* cast2(container::BinaryNode* node) {
            return static_cast<kdnode*>(node);
        }

        data_t distance(container::SampleVector* qsample) override {

            // Euclidean distance
            //data_t dist = 0;
            //for (size_t i = 0; qsample->size(); i++) {
                //(*qsample)[i] -
            //}
            //return sqrt(dist);
            return 0;
        }

        void spawn() override {

            // TODO: Boilerplate
            left = new kdnode;
            right = new kdnode;

            bestow(left);
            bestow(right);
            // End TODO: Boilerplate

            auto next_split = split_dim+1 == ioer->shape().second ?
                    0 : split_dim+1;
            cast2(left)->set_split_dim(next_split);
            cast2(left)->set_index(next_split);
            cast2(right)->set_split_dim(next_split);
            cast2(right)->set_index(next_split);


            std::vector<sample_id_t> idxs;
            data_index.get_indexes(idxs);

            std::vector<offset_t> offsets = { 0, data_index.size() / 2 };
            assert(idxs.size() == data_index.size());

            // NOTE: Always <= go left and > right
            // TODO: Better way to set split value
            assert(offsets.size() == 2);

            left->set_ph_data_index(&idxs[offsets[0]], offsets[1]-offsets[0]);
            right->set_ph_data_index(&idxs[offsets[1]],
                    idxs.size()-offsets[1]);

            // Call Schedule
            scheduler->schedule(left);
            scheduler->schedule(right);
        }

        void set_split_dim(size_t split_dim) {
            this->split_dim = split_dim;
        }

        const size_t get_split_dim() const {
            return split_dim;
        }

        // This is run next
        void run() override {
            std::cout << "\n\nkdnode at depth: " << depth << " run()\n";
            if (depth < 3) {
                sort_data_index(true); // Paralleize the sort
            } else {
                sort_data_index(false);
            }

            this->set_comparator(data_index[data_index.size() / 2 ].get_val());

#if 1
            std::cout << "Printing data from node at depth: " << depth <<
                std::endl;
            data_index.print();

            std::cout << "Comparator = " << get_comparator() << std::endl;
#endif
        }
};


// How the program runs
class kdTreeProgram: public BinaryTreeProgram {
    private:
        // All the trees in the forest (including this one!)
        std::vector<kdTreeProgram*> copse;

    public:
        // Can be used if we need no more constructors
        using BinaryTreeProgram::BinaryTreeProgram;

        NNvector find_neighbors(container::Query* q) override {
            kdnode* node = kdnode::cast2(get_root());
            container::ProximityQuery* query =
                container::ProximityQuery::raw_cast(q);

            // K nearest neighbors
            NNvector neighs (query->get_k());

            while (node) {
                auto split_dim = node->get_split_dim();
                auto dist = node->distance(query->get_qsample());
            }

            return neighs;
        }
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
    size_t nsamples = 32;
    size_t nfeatures = 16;
    tree_t ntree = 1;
    unsigned nthread = 1;
    MAT_ORIENT mo = MAT_ORIENT::COL;

    Params params(nsamples, nfeatures,
            "/Research/monya/src/test-data/rand_32_16.bin",
            IOTYPE::SYNC, ntree, nthread, mo);

    constexpr depth_t max_depth = 4;
    params.max_depth = max_depth;

    params.print();

    //utils::time t;
    ComputeEngine<kdTreeProgram>::ptr engine =
        ComputeEngine<kdTreeProgram>::create(params);
    std::cout << "Engine created ...\n";

    // Create the root with no duplicates for split dimension
    // TODO: Create root/node initializer that is passed to node
    RandomSplit rs(params.nfeatures);
    std::set<size_t> splits;

    container::BinaryNode* root = new kdnode;

    for (auto tree : engine->get_forest()) {
        while (true) {
            size_t split_dim = rs.generate();
            auto sp = splits.find(split_dim); // Pick the split dim
            if (sp == splits.end()) {
                std::cout << "Choosing split: " << split_dim << std::endl;

                // Which dim to split on
                kdnode::cast2(root)->set_split_dim(split_dim);
                kdnode::cast2(root)->set_index(split_dim); // Which samples it owns
                kdnode::cast2(root)->set_scheduler(tree->get_scheduler());

                tree->set_root(root);
                splits.insert(split_dim); // Keep track of used split dims
                break;
            }
        }
    }
    std::cout << "Roots initialized ... Training trees\n";
    engine->train();

#if 1
    std::cout << "Echoing the tree contents:\n";
    engine->get_tree(0)->echo();
#endif

#if 1
    // Query the Tree to make sure we don't have garbage!

    // Essentially replicate the data
    std::vector<data_t> tmp;
    for (size_t i = 0; i < nsamples; i++) {
        tmp.clear();
        for (size_t j  = 0; j < nfeatures; j++) {
            auto val = (data_t) ((i*nfeatures) + j);
            tmp.push_back(val);
        }

        auto qsample = container::DenseVector::create_raw(&tmp[0], tmp.size());
        container::ProximityQuery::ptr pq =
            container::ProximityQuery::create(qsample, 3); // 3-NN
        pq->print();

        engine->query(pq);

        //std::vector<container::BinaryNode*> res = pq->get_query_result();

        //res[0]->print();
        break;
    }
#endif

    return EXIT_SUCCESS;
}
