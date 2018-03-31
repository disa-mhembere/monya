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
#include "sucommon.hpp"

// Not good
#include "../RBNode.hpp"
#include "../RBTree.hpp"
#include "../BinaryNode.hpp"
#include "../BinaryTree.hpp"

namespace mc = monya::container;

namespace monya {

    namespace test {
    class NodeMapper {
        private:
        std::unordered_map<unsigned, std::vector<mc::RBNode*> > map;
        unsigned depth;

        public:
        NodeMapper() {
            depth = 0;
        }

        void init(mc::RBNode* node) {
            assert(NULL != node);
            std::vector<mc::RBNode*> v;
            v.push_back(node);
            map[depth] = v;
        }

        void insert(mc::RBNode* node) {
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
                map[++depth] = std::vector<mc::RBNode*>();
            }
        }

        void populate_tree(mc::RBTree::ptr tree) {
            assert(tree->empty());
            mc::RBNode* parent = NULL;
            for (unsigned key = 0; key < depth; key++) {
                if (key == 0) {
                    parent = map[key][0];
                    tree->insert(parent);
                }
                // TODO
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
