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

#ifndef MONYA_BINARY_NODE_HPP__
#define MONYA_BINARY_NODE_HPP__

// Represent a binary node

#include "NodeView.hpp"
#include <iostream>
#include "../common/types.hpp"

namespace monya { namespace container {

class BinaryNode: public NodeView {
    public:
        // TODO: Visibility
        // TODO: rm
#if 1
        BinaryNode* parent;
        BinaryNode* children; // Reduce # allocs when balanced
        bool left, right;
#endif
        // TODO: rm
#ifdef VECNODES
        size_t vecpos;
#endif

        // Inherit constructors
        using NodeView::NodeView;

        BinaryNode() : left(false), right(false) {
            this->depth = 0;
            req_indxs.resize(0);
        }

        BinaryNode* get_parent() { return parent; }

        static BinaryNode* cast2(NodeView* nv) {
            return static_cast<BinaryNode*>(nv);
        }

        const bool has_child() override {
            return left || right;
        }

        virtual ~BinaryNode() override {
        }
};

} } // End monya::container

#endif
