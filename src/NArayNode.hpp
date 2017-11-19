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

#ifndef NARY_NODE_HPP__
#define NARY_NODE_HPP__

// Represent an nary node

#include "NodeView.hpp"

namespace monya { namespace container {

template <typename T>
class NAryNode: public NodeView<T> {

    private:
        T data;
        child_t nchild;

    public:

        NAryNode (NodeView<T>** childs, child_t nchild) {
            this->childs = childs;
            this->nchild = nchild;
        }

        T get_data() {
            return data;
        }

        void set_data(T data) {
            this->data = data;
        }

        void add_child(NodeView<T>* child) {
        }

        static NAryNode<T>* cast2(NodeView<T>* nv) {
            return static_cast<NAryNode<T> >(nv);
        }

        void print() {
            std::cout << data << std::endl;
        }
};
} } // End monya::container

#endif
