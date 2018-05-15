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

using namespace monya;

static void* root = NULL;
class kdnode: public container::BinaryNode {
    private:
        size_t split_dim;

    public:
        // Define lineage
        kdnode* parent;
        kdnode* left;
        kdnode* right;

        // Inherit constructors
        using container::BinaryNode::BinaryNode;

        kdnode() {
            parent = left = right = NULL;
        }

        void spawn() override {

            if (depth == 0) {
                std::cout << "Root address is: " << root << std::endl;
                std::cout << "this address is: " << this << std::endl;
                assert(this == root);
            }

            // TODO: Boilerplate
            left = new kdnode;
            right = new kdnode;

            if (depth == 0) {
                assert(left == (reinterpret_cast<kdnode*>(root))->left);
                assert(right == (reinterpret_cast<kdnode*>(root))->right);
            }

            bestow(left);
            bestow(right);

#if 0
            std::cout << "Echoing LHS parent: ";
            assert(left->parent != NULL);
            left->parent->print();

            std::cout << "Echoing RHS parent: ";

            //right->parent = this;
            assert(right->parent != NULL);
            right->parent->print();
#endif
            // End TODO: Boilerplate

            auto next_split = split_dim+1 == ioer->shape().second ?
                    0 : split_dim+1;
            left->set_split_dim(next_split);
            left->set_index(next_split);
            right->set_split_dim(next_split);
            right->set_index(next_split);


            std::vector<sample_id_t> idxs;
            data_index.get_indexes(idxs);

            std::vector<offset_t> offsets = { 0, data_index.size() / 2 };
            assert(idxs.size() == data_index.size());

            // NOTE: Always <= go left and > right
            // TODO: Better way to set split value

            assert(offsets.size() == 2);

#if 0
            std::cout << "Node at depth: " << this->depth << " Spawning!\n";
            std::cout << "left indexes: ";
            io::print_arr<sample_id_t>(&idxs[offsets[0]], (offsets[1]-offsets[0]));

            std::cout << "right indexes: ";
            io::print_arr<sample_id_t>(&idxs[offsets[1]],
                    (idxs.size()-offsets[1]));
            std::cout << "\n";
#endif

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

    kdnode* _root = NULL;

    for (auto tree : engine->get_forest()) {
        while (true) {
            size_t split_dim = rs.generate();
            auto sp = splits.find(split_dim); // Pick the split dim
            if (sp == splits.end()) {
                std::cout << "Choosing split: " << split_dim << std::endl;

                root = new kdnode;
                _root = reinterpret_cast<kdnode*>(root);
                _root->set_split_dim(split_dim); // Which dim to split on
                _root->set_index(split_dim); // Which samples it owns
                _root->set_scheduler(tree->get_scheduler());

                tree->set_root(_root);
                splits.insert(split_dim); // Keep track of used split dims
                break;
            }
        }
    }
    std::cout << "Roots initialized ...\n";
    engine->train();

    assert(_root == engine->get_tree(0)->get_root());
    std::cout << "_root addr: " << _root << std::endl;
    std::cout << "engine->get_tree(0)->get_root() addr: "
        << engine->get_tree(0)->get_root() << std::endl;

    std::cout << "\n\n_root->left addr: " << _root->left << std::endl;
    std::cout << "engine->get_tree(0)->get_root()->left addr: " <<
        engine->get_tree(0)->get_root()->left  << std::endl;

    std::cout << "\n\n_root->right addr: " <<  _root->right << std::endl;
    std::cout << "engine->get_tree(0)->get_root()->right addr: " <<
        engine->get_tree(0)->get_root()->right  << std::endl;

    //assert(_root->left == engine->get_tree(0)->get_root()->left);
    //assert(_root->right == engine->get_tree(0)->get_root()->right);

#if 0
    // What does the tree look like?
    //std::cout << "Echoing the tree:\n";
    //engine->get_tree(0)->echo();

    std::cout << "Echoing the tree contents:\n";
    auto t = engine->get_tree(0);

    std::cout << "Testing the first 2 levels of the tree\n ...";
    std::cout << "Root is: \n";
    t->get_root()->print();

    std::cout << "\nRoot->left is: \n";
    assert(NULL != t->get_root()->left);
    t->get_root()->left->print();

    std::cout << "\nRoot->right is: \n";
    assert(NULL != t->get_root()->right);
    t->get_root()->right->print();
#endif

/*    // Query the Tree to make sure we don't have garbage!*/
    //std::cout << "I'm well trained!\n";

    //container::BinaryNode* bn;
    //for (unsigned i = 0; i < nsamples*nfeatures; i++) {
        //bn = new kdnode;
        //bn->set_comparator((data_t) i);

        //container::ProximityQuery::ptr pq =
            //container::ProximityQuery::create(bn, 3); // 3-NN
        //pq->print();

        //engine->query(pq);
        //std::vector<container::BinaryNode*> res = pq->get_query_result();

        //res[0]->print();
        //delete bn;
    //}

    //std::cout << "Engine trained in " << t.toc() << " seconds\n";
    return EXIT_SUCCESS;
}
