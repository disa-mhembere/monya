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
#include "../validate/BruteForcekNN.hpp"

#ifdef PROFILER
#include <gperftools/profiler.h>
#endif

using namespace monya;

class RandkdNode: public container::BinaryNode {
    private:
        size_t split_dim;
    public:
        // Inherit constructors
        using container::BinaryNode::BinaryNode;

        RandkdNode() {
            parent = left = right = NULL;
        }

        static RandkdNode* cast2(container::BinaryNode* node) {
            return static_cast<RandkdNode*>(node);
        }

        data_t distance(container::SampleVector* s1,
                const sample_id_t idx) override {
            data_t* member = ioer->get_row(idx);
            assert(NULL != member);
            data_t dist = distance::euclidean(s1->raw_data(),
                    member, s1->size());

            if (ioer->get_orientation() != ROW)
                delete [] member;
            return dist;
        }

        void spawn() override {
            // TODO: Boilerplate
            left = new RandkdNode;
            right = new RandkdNode;

            bestow(left);
            bestow(right);
            // End TODO: Boilerplate

            auto next_split = split_dim+1 == ioer->shape().second ?
                    0 : split_dim+1;
            cast2(left)->set_split_dim(next_split);
            cast2(left)->request(next_split);
            cast2(right)->set_split_dim(next_split);
            cast2(right)->request(next_split);

            std::vector<sample_id_t> idxs;
            data_index.get_indexes(idxs, get_split_dim());

            //printf("Printing idxs: ", io::print_arr(idxs));

            std::vector<offset_t> offsets = { 0,
                data_index[get_split_dim()].size() / 2 };

            // NOTE: Always <= go left and > right
            assert(offsets.size() == 2);

            left->set_ph_data_index(&idxs[offsets[0]], offsets[1]-offsets[0],
                    next_split);
            right->set_ph_data_index(&idxs[offsets[1]],
                    idxs.size()-offsets[1], next_split);
        }

        void set_split_dim(size_t split_dim) {
            this->split_dim = split_dim;
        }

        const size_t get_split_dim() const {
            return split_dim;
        }

        // This is run next
        void run() override {
            if (depth < 3) {
                sort_data_index(true); // Paralleize the sort
            } else {
                sort_data_index(false);
            }

            this->set_comparator(
                    data_index[get_split_dim()]
                    [data_index[get_split_dim()].size() / 2 ].get_val());
#if 0
            printf("Printing data from node at depth: %lu with comparator "
                    ": %.2f\n", depth, get_comparator());
            print();
#endif
        }

        void print() override {
            printf("Comparator: %.2f, Depth: %u, Split dim: %lu\n %s\n",
                    get_comparator(), depth, get_split_dim(),
                    data_index.to_string().c_str());
        }
};


// How the program runs
class RandkdTreeProgram: public BinaryTreeProgram {
    private:
        // All the trees in the forest (including this one!)
        std::vector<RandkdTreeProgram*> copse;
        std::unordered_set<container::BinaryNode*> completed;

    public:
        // Can be used if we need no more constructors
        using BinaryTreeProgram::BinaryTreeProgram;

        void find_neighbors(container::Query* q) override {
            RandkdNode* node = RandkdNode::cast2(get_root());
            container::ProximityQuery* query =
                container::ProximityQuery::raw_cast(q);
            completed.clear();

            // Keep track of visited nodes
            container::Stack<RandkdNode*> visited;

            do {
                visited.push(node);
                // compare
                auto split_dim = node->get_split_dim();
                if ((*query)[split_dim] > node->get_comparator()) {
                    node = RandkdNode::cast2(node->right);
                } else {
                    node = RandkdNode::cast2(node->left);
                }
            } while (node);
#if 1
            // TEST: Print the path the sample took
            printf("Printing the nodes in the path:\n");
            for (RandkdNode* node : visited) {
                node->print();
            }
#endif
            // Found query's closest node in tree, now compute dist from samples
            while (!visited.empty()) {
                RandkdNode* node = visited.pop(); // Now complete
                std::cout << "Popping node with comparator: "
                    << node->get_comparator() << "\n";
                // TODO: Prune step

                // Compute distance from a sample to the nodes stored here
                //  if it's a leaf

                if (!node->has_child()) {
                    for (IndexVal<data_t> iv :
                            node->get_data_index()[node->get_split_dim()]) {
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
                        visited.push(RandkdNode::cast2(node->left));
                    }

                    auto r = completed.find(node->right);
                    if (r == completed.end()) { // rhs hasn't been processed
                        std::cout << "Node c: " << node->get_comparator() <<
                            " pushing Node c: " << node->right->get_comparator()
                            << " ";
                        visited.push(RandkdNode::cast2(node->right));
                    }
                }
                completed.insert(node);
            }
        }
};

// FIXME: Use a random # generator that is ||izable
class RandomFeaturePicker {
    public:
        RandomFeaturePicker(const sample_id_t nfeatures,
                const sample_id_t npicks, std::vector<sample_id_t>& picks) {

            picks.resize(npicks);
            unsigned im = 0;

            for (unsigned in = 0; in < nfeatures && im < npicks; ++in) {
                int rn = nfeatures - in;
                int rm = npicks - im;
                if (rand() % rn < rm)
                    picks[im++] = in;
            }
        }
};

class MaxVarianceFeaturePicker {
    private:
        sample_id_t nchoose_features;

        /**
          * @param means: the feature means
          * @param vars: the feature variances
          */
        void pick_cuts(std::vector<data_t>& means, std::vector<data_t>& vars) {
            // TODO
        }

    public:
        MaxVarianceFeaturePicker(const sample_id_t nchoose_features) {
            this->nchoose_features = nchoose_features;
        }

        void pick(const sample_id_t max_sample_size,
                IndexMatrix& im, sample_id_t& cut_feature, data_t& cut_value) {

            sample_id_t sid = 0;

            // Compute feature means
            std::vector<data_t> feature_means;

            for (auto kv : im) {
                if (!feature_means.size()) // Compute mean of the featrures
                    feature_means.assign(kv.second.size(), 0); // Init @ 0
                for (size_t i = 0; i < kv.second.size(); i++) {

                    // FIXME: I need a vector not IndexVector
                    //feature_means[i] += kv.second[i];
                }
                if (++sid == max_sample_size) // Same as flann param
                    break;
            }

            // Finalize the means
            for (size_t i = 0; i < sid; i++)
                feature_means[i] /= sid;

            // Compute the feature variance
            sid = 0;
            std::vector<data_t> feature_variance;
            feature_variance.assign(feature_means.size(), 0);

            for (auto kv : im) {
                for (size_t i = 0; i < kv.second.size(); i++) {
                    // FIXME: I need a vector not an IndexVectork
                    //auto dist = kv.second[i] - feature_means[i];
                    //feature_variance[i] += dist * dist;
                }
                // Same as flann param
                if (++sid == max_sample_size)  break;
            }

            // Pick cut feature and value
            pick_cuts(feature_means, feature_variance);
        }
};

int main(int argc, char* argv[]) {
    // Positional args
    std::string datafn;
    sample_id_t nsamples;
    sample_id_t nfeatures;
    sample_id_t max_sample_size = 100; // Same as flann


    // Optional args
    tree_t ntree;
    unsigned nthread;
    depth_t max_depth;
    orient_t mo = orient_t::COL;
    constexpr unsigned FANOUT = 2;
#if 0
    bool approx = false;
    unsigned k = 3;
#endif

    datafn = "test-data/rand_32_16_cw.bin";
    nsamples = 32;
    nfeatures = 16;
    ntree = 1;
    nthread = 1;
    max_depth = 5;

#ifdef PROFILER
    ProfilerStart("rand-kdtree.perf");
#endif

    Params params(nsamples, nfeatures, datafn,
            io_t::MEM, ntree, nthread, mo, FANOUT, max_depth);
    assert(ntree < params.nfeatures);
    params.print();

    ComputeEngine<RandkdTreeProgram>::ptr engine =
        ComputeEngine<RandkdTreeProgram>::create(params);
    std::cout << "Engine created ...\n";

    utils::time timer;
    timer.tic();

#if 1
    // TODO: Create root/node initializer that is passed to node

    for (unsigned tid = 0; tid < engine->get_forest().size(); tid++) {
        auto tree = engine->get_tree(tid);
        container::BinaryNode* root = new RandkdNode;

        for (sample_id_t rid = 0; rid < std::min(nsamples, max_sample_size);
                rid++) {
            // TODO: Create multi-row request & range
            RandkdNode::cast2(root)->row_request(rid);
        }
        tree->set_root(root);
    }

    std::cout << "Roots initialized ... Training trees\n";
    engine->train();
    std::cout << "Trees trained!\n";
    std::cout << "Algorithmic time " << params.nthread << " threads: "
        <<  timer.toc() << " sec\n";
#endif

#if 1
    std::cout << "Echoing the tree contents:\n";
    for (auto tree : engine->get_forest()) {
        std::cout << "\n TREE: " << tree->get_id() << "\n";
        tree->echo();
    }
#endif

#if 0
    // Query the Tree to make sure we don't have garbage!
    std::string rw_fn =
        "/home/disa/Research/monya/src/test-data/rand_32_16_rw.bin";

    io::IO::raw_ptr syncioer = new io::SyncIO(rw_fn,
            dimpair(nsamples, nfeatures), orient_t::ROW);

    std::vector<data_t> data(nsamples*nfeatures);
    static_cast<io::SyncIO*>(syncioer)->read(&data[0]);
    validate::BruteForcekNN bf (&data[0], nsamples, nfeatures);

    // Essentially replicate the data
    std::vector<data_t> tmp;
    for (size_t i = 0; i < nsamples; i++) {
        data_t* tmp = syncioer->get_row(i);

#if 1
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

        //for (auto nnv : nnvs) {
            //assert(*nnv == iv);
        //}

        delete(container::ProximityQuery::raw_cast(pq));

#endif
        delete [] tmp;
    }
    syncioer->destroy();
#endif

#ifdef PROFILER
   ProfilerStop();
#endif
    return EXIT_SUCCESS;
}
