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
#include <algorithm>
#include <parallel/algorithm>

#include "../../SAFS/libsafs/io_interface.h"

#include "../common/types.hpp"
//#include "Scheduler.hpp"

namespace monya { namespace container {

// Represent a node in the tree
template <typename T>
class NodeView: public safs::callback {
    public:
        virtual void prep() = 0;
        virtual void run() = 0;
        virtual void init(Params&) = 0;
        virtual void spawn() = 0;
        virtual void distance(T arg1) = 0;

        // TODO: Visibility
        //std::vector<offset_t> data_index; // Indexes that nodes hold to data
        IndexVector<T> data_index; // Indexes that nodes hold to data & mapping

        // FIXME: mem waster
        std::vector<sample_id_t> req_indxs; // Indexes a vertex will req from ioer
        char* buf; // The data read from dataset
        T comparator; // The split comparator
        unsigned numbytes; // The number of bytes in the read of data from disk
        // When the data required is in memory run this computation
        short depth; // Depth of the node used as an idendifier
        // TODO: End visibility

        virtual int invoke(safs::io_request *reqs[], int num) override {
            for (int i = 0; i < num; i++) {
                this->buf = reqs[i]->get_buf();
                this->numbytes = reqs[i]->get_size();
                free(this->buf); // TODO: Verify OK
            }
            return EXIT_SUCCESS;
        }

        NodeView() {
            depth = 0;
        }

        NodeView(T val): NodeView() {
            comparator = val;
        }

        NodeView(IndexVector<T>& data_index): NodeView() {
            this->data_index = data_index;
        }

        // Range index
        virtual void set_index(sample_id_t start_idx,
                const sample_id_t nsamples) {
            for (sample_id_t idx = start_idx; idx < start_idx; idx++)
                req_indxs.push_back(idx);
        }

        // Iterative index
        void set_index(const std::vector<sample_id_t>& indexes) {
                req_indxs = indexes; // TODO: Verify copy
        }

        void set_depth(short depth) {
            this->depth = depth;
        }

        const short get_depth() const {
            return depth;
        }

        void get_data() {
            // TODO
        }

        void sort_data_index(bool par=false) {
            if (par)
                __gnu_parallel::sort(data_index.begin(),
                        data_index.end());
            else
                std::sort(data_index.begin(), data_index.end());
        }

        virtual const IndexVector<T>& get_data_index() const {
            return data_index;
        }

        virtual const void print() const {
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

        bool is_root() {
            return depth == 0;
        }

        virtual ~NodeView() {
        };
};

template <typename T> std::ostream& operator<<
    (std::ostream& stream, const NodeView<T>& node) {
    stream << node.comparator;
    return stream;
}

} } // End monya::container
#endif
