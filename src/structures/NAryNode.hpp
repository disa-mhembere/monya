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

#include <exception>
#include <iostream>
#include "NodeView.hpp"

namespace monya { namespace container {

// TODO: Complete
template <typename T>
static bool powerof(T num, size_t pow) {

    if (pow == 2) {
        // TODO
    } else {
        throw std::runtime_error("Not implemented yet!");
    }

    return false;
}

template <typename T>
class NAryNode: public NodeView<T> {

    private:
        T data;
        child_t nchild;

        void resize_child_container(size_t to) {
            // TODO
        }

        void grow_container() {
            // TODO
        }

        void shrink_container() {
            // TODO
        }

    public:

        NAryNode () {
            this->nchild = 0;
        }

        NAryNode (NodeView<T>** childs, child_t nchild,
                child_t container_size) {
            /**
              * \param container_size: the length of the array containing children
              */
            this->childs = childs;
            this->nchild = nchild;
            if (!powerof(container_size, 2)) {
                // TODO
            }
        }

        T get_data() {
            return data;
        }

        void set_data(T data) {
            this->data = data;
        }

        void add_child(NodeView<T>* child) {
            // TODO
        }

        void remove_child() {
            // TODO
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
