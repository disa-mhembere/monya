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
//#include "../common/cxxopts/cxxopts.hpp"

#ifdef PROFILER
#include <gperftools/profiler.h>
#endif

using namespace monya;

class kdnode: public container::BinaryNode {
    private:
        size_t split_dim;
#ifdef PRUNE
        std::vector<data_t> upper_bounds, lower_bounds;
#endif
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

            if (ioer->get_orientation() != ROW)
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

        // TODO: ||ize
#ifdef PRUNE
        void compute_bounds() {
            // Upper and lower bounds at this node
            lower_bounds.assign(ioer->shape().second,
                    std::numeric_limits<data_t>::max());
            upper_bounds.assign(ioer->shape().second,
                    std::numeric_limits<data_t>::min());

            for (auto iv : data_index[get_split_dim()]) {
                data_t* sample = ioer->get_row(iv.get_index());

                for (size_t feat_id = 0; feat_id < ioer->shape().second;
                        feat_id++) {
                    if (sample[feat_id] < lower_bounds[feat_id])
                        lower_bounds[feat_id] = sample[feat_id];
                    if (sample[feat_id] > upper_bounds[feat_id])
                        upper_bounds[feat_id] = sample[feat_id];
                }

                if (ioer->get_orientation() != ROW)
                    delete [] sample; // TODO: Coz wrong access pattern
            }
        }
#endif

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
#ifdef PRUNE
            // What makes pruning possible
            compute_bounds();
#endif
        }

        void print() override {
            printf("Comparator: %.2f, Depth: %lu, Split dim: %lu\n %s\n",
                    get_comparator(), depth, get_split_dim(),
                    data_index.to_string().c_str());
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
        RandomSplit(size_t nfeatures) {
            distribution = std::uniform_int_distribution<size_t>(0, nfeatures);
            distribution(generator); // Throw away the first one
        }

        size_t generate() {
            return distribution(generator);
        }
};

int main(int argc, char* argv[]) {
    // Positional args
    std::string datafn;
    size_t nsamples;
    size_t nfeatures;

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
    nthread = 4;
    max_depth = 5;

#ifdef PROFILER
    ProfilerStart("kdtree.perf");
#endif

#if 0
    try {
        cxxopts::Options options(argv[0],
                "kdtree data-file nsamples nfeatures [alg-options]\n");
        options.positional_help("[optional args]");

        options.add_options()
            ("f,datafn", "Path to data-file on disk",
             cxxopts::value<std::string>(datafn), "FILE")
            ("n,nsamples", "Number of samples in the dataset (rows)",
             cxxopts::value<std::string>())
            ("m,nfeatures", "Number of features in the dataset (columns)",
             cxxopts::value<std::string>())
            ("t,ntree", "Number of trees in the forest",
             cxxopts::value<tree_t>(ntree)->default_value("1"))
            ("T,num_thread", "The number of threads to run",
             cxxopts::value<unsigned>(nthread)->default_value("1"))
            ("d,depth", "Max tree depth",
             cxxopts::value<depth_t>(max_depth)->default_value("10"))
            ("o,orientation", "data orientataion `row` or `col`)",
             cxxopts::value<std::string>()->default_value("col"))
            ("A,approx", "Do approx rather than exact kNN",
             cxxopts::value<bool>(approx))
            ("k,num_neighs", "The number of nearest neighbors",
             cxxopts::value<unsigned>(k)->default_value("3"))
            ("h,help", "Print help");

        options.parse_positional({"datafn", "nsamples", "nfeatures"});
        int nargs = argc;
        options.parse(argc, argv);

        if (options.count("help") || (nargs == 1)) {
            std::cout << options.help() << std::endl;
            exit(EXIT_SUCCESS);
        }

        if (nargs < 4) {
            std::cout << "[ERROR]: Not enough default arguments\n";
            std::cout << options.help() << std::endl;
            exit(EXIT_SUCCESS);
        }

        nsamples = atol(options["nsamples"].as<std::string>().c_str());
        nfeatures = atol(options["nfeatures"].as<std::string>().c_str());
        mo = options["orientation"].as<std::string>() == "col" ? COL : ROW;

    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }
#endif

    Params params(nsamples, nfeatures, datafn,
            io_t::MEM, ntree, nthread, mo, FANOUT, max_depth);
    assert(ntree < params.nfeatures);
    params.print();

    ComputeEngine<kdTreeProgram>::ptr engine =
        ComputeEngine<kdTreeProgram>::create(params);
    std::cout << "Engine created ...\n";

    utils::time timer;
    timer.tic();

    // Create the root with no duplicates for split dimension
    // TODO: Create root/node initializer that is passed to node
    RandomSplit rs(params.nfeatures);

    // Pick the first tree's split dim
    cunsigned split_dim(rs.generate(), params.nfeatures);

    for (size_t tid = 0; tid < engine->get_forest().size(); tid++) {
        auto tree = engine->get_tree(tid);

        std::cout << "Tree: " << tree->get_id() <<
            " INIT Choosing split: " << split_dim.get() << std::endl;
        container::BinaryNode* root = new kdnode;

        // Which dim to split on
        kdnode::cast2(root)->set_split_dim(split_dim.get());
        // Which col to get from the data & place in memory
        kdnode::cast2(root)->request(split_dim.get());
        tree->set_root(root);
        split_dim.inc();
    }

    std::cout << "Roots initialized ... Training trees\n";
    engine->train();
    std::cout << "Trees trained!\n";
    std::cout << "Algorithmic time " << params.nthread << " threads: "
        <<  timer.toc() << " sec\n";

#if 0
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
