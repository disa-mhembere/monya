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
#include "../common/exception.hpp"
#include "../common/types.hpp"

namespace monya { namespace container {

class BinaryNode: public NodeView {
    public:
        // TODO: Visibility
        // TODO: rm
#if 1
        BinaryNode* parent;
        BinaryNode* left;
        BinaryNode* right;
#endif

        // TODO: rm
#if 1
        size_t vecpos;
#endif

        // Inherit constructors
        using NodeView::NodeView;

        BinaryNode* get_parent() {
            return parent;
        }

        virtual void run() override {
            throw abstract_exception("BinaryNode::run");
        }

        virtual void distance(data_t arg1) override {
            throw abstract_exception("BinaryNode::distance");
        }

        virtual void init(Params&) override {
            throw abstract_exception("BinaryNode::init");
        }

        static BinaryNode* cast2(NodeView* nv) {
            return static_cast<BinaryNode*>(nv);
        }

        void read_svm() {
            // TODO
        }

        // Async writeback
        void persist() {
            // TODO
        }

        // Keep the node in memory
        void cache() {
            // TODO
        }

        ~BinaryNode() override {
        }
};

} } // End monya::container

#endif
