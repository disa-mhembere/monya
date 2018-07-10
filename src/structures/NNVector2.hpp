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

#ifndef MONYA_NNVECTOR_HPP__
#define MONYA_NNVECTOR_HPP__

#include "../common/types.hpp"

namespace monya { namespace container {

class NNVector2 {
    private:
        class Node {
            protected:
                Node* next;
                Node* prev;
                data_t data;
            public:
                Node() : next(NULL), prev(NULL),
                    data(std::numeric_limits<data_t>::max()) { }

                Node(data_t data) : Node() {
                    this->data = data;
                }
        };

        Node* sentinel;
        sample_id_t k;
        size_t _size;

        void push_back(Node* new_node) {
            if (!_size) {
                new_node->next = new_node->prev = sentinel;
                sentinel->next = sentinel->prev = new_node;
            } else {
                // Push back
                Node* back = sentinel->prev;
                new_node->next = sentinel;
                new_node->prev = back;
                back->next = new_node;
                sentinel->prev = new_node;
            }
            _size++;
        }

    public:
        NNVector2(sample_id_t _k) : NNVector2() {
            this->k = k;
            sentinel = new Node();
            size = 0;
        }

        /**
          * First determine if the val falls with the kNN at all.
          *     If so add in the correct position, else ignore
          */
        void eval(data_t val) {
            if (!k) // O-NN
                return;

            Node* curr = sentinel;
            do {
                curr = curr->next;
                if (curr == sentinel) { // Empty list
                   Node* new_node = Node(val);
                   new_node->prev = curr;
                   new_node->next = curr;
                   curr->next = new_node;
                   curr->prev = new_node;
                } else {
                }

            } while (node != sentinel);

        }

        size_t size() {
            return _size;
        }

        ~NNVector2() {
            Node* node = sentinel;

            do {
                // TODO: delete nodes
            } while (node != sentinel);

            delete sentinel;
        }
};

}} // End namespace monya::container

#endif
