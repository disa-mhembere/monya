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
#include <unordered_set>
#include <set>

#include "../common/monya.hpp"
#include "../utils/time.hpp"
#include "../io/IO.hpp"
#include "../structures/SampleVector.hpp"
#include "../validate/BruteForcekNN.hpp"

using namespace monya;

class kdnode: public container::BinaryNode {
    private:
        size_t split_dim;
        std::vector<data_t> upper_bounds, lower_bounds;
        bool complete;

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
            lower_bounds.assign(ioer->shape().second,
                    std::numeric_limits<data_t>::max());
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
            printf("\n\nkdnode at depth: %lu run()\n", depth);
            if (depth < 3) {
                sort_data_index(true); // Paralleize the sort
            } else {
                sort_data_index(false);
            }

            this->set_comparator(data_index[data_index.size() / 2 ].get_val());
#if 1
            printf("Printing data from node at depth: %lu with comparator "
                    ":%.2f\n", depth, get_comparator());
#endif

#if 1
            // What makes pruning possible
            compute_bounds();
#endif
        }

        void print() override {
            printf("Comparator: %.2f, Split dim: %lu\n %s\n", get_comparator(),
            get_split_dim(), data_index.to_string().c_str());
            printf("Membership: %s\n",  data_index.to_string().c_str());
#if 0
            std::cout << "Upper bounds:\n";
            io::print_arr<data_t>(&upper_bounds[0], upper_bounds.size());
            std::cout << "Lower bounds:\n";
            io::print_arr<data_t>(&lower_bounds[0], lower_bounds.size());
#endif
            std::cout << "\n";
        }
};


// How the program runs
class kdTreeProgram: public BinaryTreeProgram {
    private:
        // All the trees in the forest (including this one!)
        std::vector<kdTreeProgram*> copse;
        std::unordered_set<container::BinaryNode*> completed;

    public:
        // Can be used if we need no more constructors
        using BinaryTreeProgram::BinaryTreeProgram;

        void find_neighbors(container::Query* q) override {
            kdnode* node = kdnode::cast2(get_root());
            container::ProximityQuery* query =
                container::ProximityQuery::raw_cast(q);
            completed.clear();

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
#if 1
            // TEST: Print the path the sample took
            printf("Printing the nodes in the path:\n");
            for (kdnode* node : visited) {
                node->print();
            }
#endif
            // Found query's closest node in tree, now compute dist from samples
            while (!visited.empty()) {
                kdnode* node = visited.pop(); // Now complete
                std::cout << "Popping node with comparator: "
                    << node->get_comparator() << "\n";
                // TODO: Prune step

                // Compute distance from a sample to the nodes stored here
                //  if it's a leaf

                if (!node->has_child()) {
                    for (IndexVal<data_t> iv : node->get_data_index()) {
                        auto dist = node->distance(&(query->get_qsample()[0]),
                                iv.get_index());
                        std::cout << "\n\nDist to: " << iv.get_index() <<
                            " = " << dist << std::endl;

                        query->eval(iv.get_index(), dist, tree_id);
                    }
                } else {
                    auto l = completed.find(node->left);
                    if (l == completed.end()) { // lhs hasn't been processed
                        std::cout << "Node c: " << node->get_comparator() <<
                            " pushing Node c: " <<
                            node->left->get_comparator() << "\n";
                        visited.push(kdnode::cast2(node->left));
                    }

                    auto r = completed.find(node->right);
                    if (r == completed.end()) { // rhs hasn't been processed
                        std::cout << "Node c: " << node->get_comparator() <<
                            " pushing Node c: " << node->right->get_comparator()
                            << " ";
                        visited.push(kdnode::cast2(node->right));
                    }
                }
                completed.insert(node);
            }
        }
};

class RandomSplit {
    private:
        std::default_random_engine generator;
        std::uniform_int_distribution<size_t> distribution;
    public:
        RandomSplit(size_t nfeatures, size_t npick) {
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
    unsigned nthread = atoi(argv[1]);
    MAT_ORIENT mo = MAT_ORIENT::COL;

    Params params(nsamples, nfeatures,
            "/home/disa/Research/monya/src/test-data/rand_32_16_cw.bin",
            IOTYPE::MEM, ntree, nthread, mo);

    constexpr depth_t max_depth = 5;
    params.max_depth = max_depth;

    params.print();

    ComputeEngine<kdTreeProgram>::ptr engine =
        ComputeEngine<kdTreeProgram>::create(params);
    std::cout << "Engine created ...\n";

    // Create the root with no duplicates for split dimension
    // TODO: Create root/node initializer that is passed to node
    RandomSplit rs(params.nfeatures, ntree);
    std::set<size_t> splits;

    for (auto tree : engine->get_forest()) {
        while (true) {
            size_t split_dim = rs.generate();
            auto sp = splits.find(split_dim); // Pick the split dim
            if (sp == splits.end()) {
                std::cout << "Tree: " << tree->get_id() <<
                    " INIT Choosing split: " << split_dim << std::endl;

                container::BinaryNode* root = new kdnode;

                // Which dim to split on
                kdnode::cast2(root)->set_split_dim(split_dim);
                // Which col to get from the data & place in memory
                kdnode::cast2(root)->set_index(split_dim);
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
    for (auto tree : engine->get_forest()) {
        std::cout << "\n TREE: " << tree->get_id() << "\n";
        tree->echo();
    }
#endif

#if 1
    // Query the Tree to make sure we don't have garbage!
    std::string rw_fn = "/home/disa/Research/monya/src/test-data/rand_32_16_rw.bin";

    io::IO::raw_ptr syncioer = new io::SyncIO(rw_fn,
            dimpair(nsamples, nfeatures), MAT_ORIENT::ROW);

    std::vector<data_t> data(nsamples*nfeatures);
    static_cast<io::SyncIO*>(syncioer)->read(&data[0]);
    validate::BruteForcekNN bf (&data[0], nsamples, nfeatures);

    // Essentially replicate the data
    std::vector<data_t> tmp;
    for (size_t i = 0; i < nsamples; i++) {
        data_t* tmp = syncioer->get_row(i);

#if 1
        constexpr unsigned k = 5;
        auto qsample = container::DenseVector::create_raw(tmp, nfeatures);
        container::Query* pq = new container::ProximityQuery(qsample, k, ntree);

        std::cout << "Sample: " << i << " " << k << "NN:\n";
        pq->print();

        engine->query(pq);
        std::cout << "Monya Brute: \n";
        std::vector<NNVector*> nnvs =
            container::ProximityQuery::raw_cast(pq)->getNN();

        assert(nnvs.size() == ntree);
        nnvs[0]->print();

        auto iv = bf.getNN(tmp, k);
        std::cout << "Brute: \n";
        iv.print();

        if (ntree > 1)
            for(size_t i = 1; i < ntree; i++)
                assert(*(nnvs[0]) == (*nnvs[1]));

        for (auto nnv : nnvs)
            assert(*nnv == iv);

        delete(container::ProximityQuery::raw_cast(pq));

#endif
        delete [] tmp;
    }
    syncioer->destroy();
#endif

    return EXIT_SUCCESS;
}
