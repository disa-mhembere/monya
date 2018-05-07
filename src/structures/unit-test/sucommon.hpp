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

#ifndef MONYA_SUCOMMON_HPP__
#define MONYA_SUCOMMON_HPP__

#include <unordered_map>
#include <vector>
#include <cmath>
#include <memory>

#include "sucommon.hpp"

// Not good
#include "../RBTree.hpp"
#include "../BinaryNode.hpp"
#include "../BinaryTree.hpp"

namespace mc = monya::container;

namespace monya {

    // A class to allow author to build a specific tree with specific
    //  structure
    namespace test {
    class NodeMapper {
        public:
        std::unordered_map<unsigned, std::vector<mc::BinaryNode*> > map;
        unsigned depth;

        NodeMapper() {
            depth = 0;
        }

        void init(mc::BinaryNode* node) {
            assert(NULL != node);
            std::vector<mc::BinaryNode*> v;
            v.push_back(node);
            map[depth] = v;
        }

        void insert(mc::BinaryNode* node) {
            assert(NULL != node);

            if (map.empty()) {
                init(node);
                return;
            } else {
                check();
                map[depth].push_back(node);
            }
        }

        void check() {
            if (std::pow(2, depth) == map[depth].size()) {
                map[++depth] = std::vector<mc::BinaryNode*>();
            }
        }

        const size_t size() {
            return map.size();
        }

        void populate_tree(std::shared_ptr<mc::BinaryTree> tree) {
            constexpr unsigned FANOUT = 2;
            assert(tree->empty());
            mc::BinaryNode* parent = NULL;

            for (unsigned key = 0; key < map.size(); key++) {
                if (key == 0) {
                    parent = map[key][0];
                    tree->insert(parent); // root

#ifdef MONYA_VERBOSE
                    std::cout << "Inserting root: "; parent->print();
                    std::cout << "\n";
#endif
                } else {
                    // All other levels
                    auto nodes = map[key]; // vector of nodes on the level
                    mc::BinaryNode* left = NULL;
                    mc::BinaryNode* right = NULL;

                    for (size_t id = 0; id < nodes.size(); id+=FANOUT) {
                        parent = map[key-1][id/FANOUT];

                        // These shouldn't be populated
                        assert(NULL == parent->left);
                        assert(NULL == parent->right);

#ifdef MONYA_VERBOSE
                        std::cout << "Parent node: "; parent->print();
                        std::cout << "\n";
#endif

                        if (*nodes[id] < *nodes[id+1]) {
                            left = nodes[id];
                            right = nodes[id+1];
                        } else {
                            left = nodes[id+1];
                            right = nodes[id];
                        }

#ifdef MONYA_VERBOSE
                        std::cout << "Inserting left as: "; left->print();
                        std::cout << "\n";
                        std::cout << "Inserting right as: "; right->print();
                        std::cout << "\n";
#endif

                        // Set left and right
                        tree->insert_at(left, parent, bchild_t::LEFT);
                        tree->insert_at(right, parent, bchild_t::RIGHT);
                    }
                }
            }
        }

        void print() {
            for (unsigned key = 0; key <= depth; key++) {
                std::cout << "level: " << key << std::endl;
                for (auto it = map[key].begin(); it != map[key].end(); ++it) {
                    std::cout << (*it)->get_comparator() << " ";
                }
                std::cout << "\n\n";
            }
        }
    };
} }

#endif
