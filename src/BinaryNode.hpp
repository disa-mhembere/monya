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

#ifndef BINARY_NODE_HPP__
#define BINARY_NODE_HPP__

// Represent a binary node

#include "NodeView.hpp"
#include <iostream>

namespace monya { namespace container {

template <typename T>
class BinaryNode: public NodeView<T> {

    private:
        T data;

    public:

        BinaryNode (T data, NodeView<T>* left=NULL, NodeView<T>* right=NULL) {
            this->data = data;
            this->left;
            this->right;
        }

        void set_data(T data) {
            this->data = data;
        }

        void set_left(NodeView<T>* left) {
            this->left = left;
        }

        void set_right(NodeView<T>* right) {
            this->right = right;
        }

        T get_data() {
            return data;
        }

        static BinaryNode<T>* cast2(NodeView<T>* nv) {
            return static_cast<BinaryNode<T> >(nv);
        }

        void print() {
            std::cout << data << std::endl;
        }
};
} } // End monya::container

#endif
