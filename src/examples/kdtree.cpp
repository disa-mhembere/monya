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
        std::vector<data_t> upper_bounds, lower_bounds;

    public:
        // Inherit constructors
        using container::BinaryNode::BinaryNode;

        kdnode() {
            parent = left = right = NULL;
        }

        static kdnode* cast2(container::BinaryNode* node) {
            return static_cast<kdnode*>(node);
        }

        data_t distance(container::SampleVector* s1,
                const sample_id_t idx) override {
            data_t* member = ioer->get_row(idx);
            assert(NULL != member);
            data_t dist = distance::euclidean(s1->raw_data(),
                    member, s1->size());
            delete [] member;
            return dist;
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

        // TODO: ||ize
        void compute_bounds() {
            // Upper and lower bounds at this node
            //lower_bounds.resize(ioer->shape().second);
            lower_bounds.assign(ioer->shape().second,
                    std::numeric_limits<data_t>::max());
            //upper_bounds.resize(ioer->shape().second);
            upper_bounds.assign(ioer->shape().second,
                    std::numeric_limits<data_t>::min());

            for (auto iv : data_index) {
                data_t* sample = ioer->get_row(iv.get_index());

                for (size_t feat_id = 0; feat_id < ioer->shape().second;
                        feat_id++) {
                    if (sample[feat_id] < lower_bounds[feat_id])
                        lower_bounds[feat_id] = sample[feat_id];
                    if (sample[feat_id] > upper_bounds[feat_id])
                        upper_bounds[feat_id] = sample[feat_id];
                }
                delete [] sample; // TODO: Coz wrong access pattern
            }
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

#if 1
            // What makes pruning possible
            compute_bounds();
#endif
        }

        const void print() const override {
            std::cout << "Comparator: " << get_comparator() << "\n";
            std::cout << "Membership: \n"; data_index.print();
            std::cout << "Upper bounds:\n";
            io::print_arr<data_t>(&upper_bounds[0], upper_bounds.size());
            std::cout << "Lower bounds:\n";
            io::print_arr<data_t>(&lower_bounds[0], lower_bounds.size());
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

#if 1
        NNvector find_neighbors(container::Query* q) override {
            kdnode* node = kdnode::cast2(get_root());
            container::ProximityQuery* query =
                container::ProximityQuery::raw_cast(q);

            // K nearest neighbors
            NNvector neighs;
            if (empty())
                return neighs;
            else
                neighs.resize(query->get_k());

            // Keep track of visited nodes
            container::Stack<kdnode*> visited;

            do {
                visited.push(node);
                // compare
                auto split_dim = node->get_split_dim();
                if ((*query)[split_dim] > node->get_comparator()) {
                    node = kdnode::cast2(node->right);
                } else {
                    node = kdnode::cast2(node->left);
                }
            } while (node);

#if 0
            // TEST: Print the path the sample took
            printf("Printing the nodes in the path:\n");
            for (kdnode* node : visited) {
                node->print();
            }
#endif
            // Found query's closest node in tree, now compute dist from samples
            while (!visited.empty()) {
                kdnode* node = visited.pop(); // Now complete
                // Compute distance from a sample to the nodes stored here
                //  if it's a leaf

                if (!node->has_child()) {
                    for (IndexVal<data_t> iv : node->get_data_index()) {
                        auto dist = node->distance(&(query->get_qsample()[0]),
                                iv.get_index());
                        std::cout << "\n\nDist to: " << iv.get_index() <<
                            " = " << dist << std::endl;

                        // TODO: DS that does work to place nearest neighbors in
                    }
                }
            }

            return neighs;
        }
#endif
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
            "/Research/monya/src/test-data/rand_32_16_cw.bin",
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

    io::IO::raw_ptr syncioer = new io::SyncIO(
            "/Research/monya/src/test-data/rand_32_16_rw.bin",
            dimpair(nsamples, nfeatures), MAT_ORIENT::ROW);

    // Essentially replicate the data
    std::vector<data_t> tmp;
    for (size_t i = 0; i < nsamples; i++) {
        data_t* tmp = syncioer->get_row(i);
        std::cout << "Sample v" << i << ":\n";
        io::print_arr<data_t>(tmp, nfeatures);

#if 1
        auto qsample = container::DenseVector::create_raw(tmp, nfeatures);
        container::ProximityQuery::ptr pq =
            container::ProximityQuery::create(qsample, 3); // 3-NN
        pq->print();

        engine->query(pq);

        //std::vector<container::BinaryNode*> res = pq->get_query_result();

        //res[0]->print();
#endif
        delete [] tmp;
        break;
    }
    syncioer->destroy();
#endif

    return EXIT_SUCCESS;
}
