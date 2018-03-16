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
#include "io_interface.h"
#include "../common/exception.hpp"

namespace monya { namespace container {

template <typename T>
class BinaryNode: public NodeView<T> {
    public:
        // TODO: Visibility
        BinaryNode<T>* parent;
        BinaryNode<T>* left;
        BinaryNode<T>* right;

        // Inherit constructors
        using NodeView<T>::NodeView;
        BinaryNode<T>* get_parent() {
        }

        // Given a new node add children to this node
        virtual void spawn() override {
            throw abstract_exception("BinaryNode::spawn");
#if 0
            if (node->get_data() < this->data) {
                if (!l)  // No left child
                    left(node);  // Make node my left child
                else
                    l->spawn(node);
            } else if (node->get_data() > this->data) {
                if (!r)  // No right child
                    right(node);
                else
                    r->spawn(node);
            }
#endif
        }

        virtual void prep() override {
            throw abstract_exception("BinaryNode::prep");
        }

        virtual void run() override {
            throw abstract_exception("BinaryNode::run");
        }

        virtual void distance(T arg1) override {
            throw abstract_exception("BinaryNode::distance");
        }

        virtual void init(Params&) override {
            throw abstract_exception("BinaryNode::init");
        }

        static BinaryNode<T>* cast2(NodeView<T>* nv) {
            return static_cast<BinaryNode<T>* >(nv);
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
