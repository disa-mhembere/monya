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

#ifndef MONYA_NODEVIEW_HPP__
#define MONYA_NODEVIEW_HPP__

#include <memory>
#include <utility>
#include <iostream>

#include "io_interface.h"
#include "../common/types.hpp"

namespace monya { namespace container {

// Represent a node in the tree
template <typename T>
class NodeView: public safs::callback {
    protected:
        virtual void prep() = 0;
        virtual void run() = 0;
    public:
        // TODO: Visibility
        std::vector<offset_t> data_index; // Indexes that nodes hold to data
        char* buf; // The data read from dataset
        T comparator; // The split comparator
        unsigned numbytes; // The number of bytes in the read of data from disk
        // When the data required is in memory run this computation
        // TODO: End visibility

        virtual int invoke(safs::io_request *reqs[], int num) override {
            for (int i = 0; i < num; i++) {
                this->buf = reqs[i]->get_buf();
                this->numbytes = reqs[i]->get_size();
                free(this->buf); // TODO: Verify OK
            }
            return EXIT_SUCCESS;
        }

        NodeView() {}

        NodeView(T val) {
            comparator = val;
        }

        NodeView(std::vector<offset_t>& data_index) {
            this->data_index = data_index;
        }

        NodeView(offset_t* data_index, const size_t nelem) {
            add_elem(data_index, nelem);
        }

        void add_elem(offset_t* members, const size_t nelem) {
            if (data_index.size()) { // We have members so add to them
                data_index.insert(data_index.end(), members, members+nelem);
            } else {
                data_index.resize(nelem);
                std::copy(members, members+nelem, data_index.begin());
            }
        }

        virtual void spawn(NodeView<T>* node) = 0;

        virtual const std::vector<offset_t>& get_data_index() const {
            return data_index;
        }

        void set_data_index(const std::vector<offset_t>& data_index) {
            this->data_index = data_index;
        }

        void set_data_index(const offset_t* data_index, const size_t nelem) {
            add_elem(data_index, nelem);
        }

        const void print() const {
            std::cout << comparator << std::endl;
        }

        const T get_comparator() const {
            return comparator;
        }

        void set_comparator(const T comparator) {
            this->comparator = comparator;
        }

        virtual bool operator==(const NodeView<T>& other) {
            return comparator == other.get_comparator();
        }

        virtual bool operator!=(const NodeView<T>& other) {
            return !(*this == other);
        }

        virtual bool operator<(const NodeView<T>& other) {
            return comparator < other.get_comparator();
        }

        virtual bool operator>(const NodeView<T>& other) {
            return this->get_comparator() > other.get_comparator();
        }

        virtual bool operator<=(const NodeView<T>& other) {
            return !(*this > other);
        }

        virtual bool operator>=(const NodeView<T>& other) {
            return !(*this < other);
        }

        virtual ~NodeView() {
        };
};

template <typename T> std::ostream& operator<<
    (std::ostream& stream, const NodeView<T>& node) {
    stream << node.comparator;
    return  stream;
}

} } // End monya::container
#endif
