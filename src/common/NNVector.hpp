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

#include "types.hpp"
#include "exception.hpp"

namespace monya {

class NNVector {
    private:

        class Node {
            public:
                Node* prev;
                Node* next;
                IndexVal<data_t> data;

                Node() : prev(NULL), next(NULL) { }
                Node(IndexVal<data_t>& data) : Node() {
                    this->data = data;
                }

                void print() {
                    if (data.get_val() == std::numeric_limits<data_t>::max())
                        std::cout << "sentinel\n";
                    else
                        data.print();
                }

                std::string to_string() {
                    if (data.get_val() == std::numeric_limits<data_t>::max())
                        return "sentinel";
                    else
                        return data.to_string();
                }
        };

        Node* sentinel;
        std::pair<size_t, Node*> cache; // consecutive access optimization
        sample_id_t k;
        size_t _size;

        void push_back(Node* new_node) {
#if 0
            printf("pushing back node: "); new_node->print();
#endif

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

        void update_cache(size_t index, Node* node) {
            // Update the cache
            cache.first = index;
            cache.second = node;
        }

        void invalidate_cache() {
            if (cache.second)
                cache.second = NULL;
        }

    public:
        NNVector(sample_id_t k) {
            this->k = k;
            sentinel = new Node();
            sentinel->next = sentinel->prev = sentinel;
            sentinel->data = IndexVal<data_t>(
                    0, std::numeric_limits<data_t>::max());
            _size = 0;
        }

        void pop_back() {
#if 0
            printf("Popping the back!\n");
#endif

            if (!_size)
                return; // do nothing

            Node* delnode = sentinel->prev;
            if (delnode == sentinel)
                return; // do nothing (size 1 list)

            delnode->prev->next = sentinel;
            sentinel->prev = delnode->prev;
            delete delnode;
            _size--; // Decrement the structure size

            invalidate_cache();
        }

        void eval(const sample_id_t id, data_t dist) {
            eval(IndexVal<data_t> (id, dist));
        }

        /**
          * First determine if the val falls within the kNN at all.
          *     If so add in the correct position, else ignore
          */
        // TODO: Copies :(
        void eval(IndexVal<data_t> iv) {
            if (!size()) {
#if 0
                printf("Adding to empty list: "); iv.print();
#endif
                Node* new_node = new Node(iv);
                sentinel->next = new_node;
                new_node->prev = sentinel;
                sentinel->prev = new_node;
                new_node->next = sentinel;
                _size++;
            } else {
                data_t val = iv.get_val();
                Node* curr = sentinel->next;
                bool insert_flag = false;

                while (curr != sentinel) {
                    assert(NULL != curr);

                    // NOTE: Strictly less than!
                    if (val < curr->data.get_val()) {
                        Node* new_node = new Node(iv);

#if 0
                        std::cout << "Inserting new node:"
                            << iv.to_string() << " between: " <<
                            curr->prev->to_string() << " and " <<
                            curr->to_string() << "\n";
#endif

                        curr->prev->next = new_node;
                        new_node->prev = curr->prev;

                        curr->prev = new_node;
                        new_node->next = curr;
                        insert_flag = true;

                        _size++;
                        break;
                    }
                    curr = curr->next;
                }

                // New node is the last element in the list
                if (_size < k && !insert_flag)
                    push_back(new Node(iv));
            }

            // We've inserted in between the 1st & last nodes & have too many
            if (_size > k)
                pop_back();

            invalidate_cache();
        }

        bool operator==(IndexVector& iv) {
            if (iv.size() != size())
                return false;

            for (size_t i = 0; i < size(); i++) {
                if (iv[i] != (*this)[i])
                    return false;
            }

            return true;
        }

        // TODO: Could be faster
        bool operator==(NNVector* other) {
            if (other->size() != size())
                return false;

            for (size_t i = 0; i < size(); i++) {
                if ((*other)[i] != (*this)[i])
                    return false;
            }

            return true;
        }

        IndexVal<data_t>& operator[](const size_t index) {
            if ((index + 1) > _size)
                throw std::runtime_error(std::string("Out of bounds index: ")
                        + std::to_string(index));

            Node* curr = NULL;

            // Warm cache
            if (cache.second != NULL) {

                if (cache.first == index - 1) {
                    // Cache hit
                    curr = cache.second->next;

                    // Update the cache
                    update_cache(index, curr);
                } else {
                    size_t pos = 0;

                    // Cache miss & update (sequential case)
                    if (index >= cache.first) {
                        pos = cache.first;
                        curr = cache.second;
                        while(pos++ < index)
                            curr = curr->next;
                    } else  { // non-sequential
                        pos = 0;
                        curr = sentinel;

                        while (pos++ <= index)
                            curr = curr->next;
                    }

                    // Update the cache
                    update_cache(index, curr);
                }
            } else { // Cold cache
                curr = sentinel;
                size_t pos = 0;
                while (pos++ <= index)
                    curr = curr->next;

                // Update the cache
                update_cache(index, curr);
            }

            assert(NULL != curr);
            return curr->data;
        }

        IndexVector to_IndexVector() {
            IndexVector ret;
            Node* curr = sentinel->next;
            while(curr != sentinel)
                ret.append(curr->data);
            return ret;
        }

        bool find(const IndexVal<data_t>& iv) {
            Node* curr = sentinel->next;
            while (curr != sentinel) {
                if (curr->data == iv) {
                    return true;
                }
            }

            return false;
        }

        const void print() const {
            printf("Printing list with size: %lu\n", _size);
            Node* curr = sentinel->next;

            while (curr != sentinel) {
                curr->print();
                curr = curr->next;
                assert(NULL != curr);
            }
        }

        const size_t size() const {
            return _size;
        }

        const bool empty() const {
            return _size == 0;
        }

        ~NNVector() {
#if 0
            printf("Destructing NNVector\n");
#endif
            if (_size) {

                Node* curr = sentinel->next;
                Node* curr_next = curr->next;

                while (true) {
                    delete curr;

                    curr = curr_next;
                    if (curr == sentinel)
                        break;
                    curr_next = curr->next;
                }
            }

            delete sentinel;
        }
};

} // End namespace monya:
#endif
