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

#include "NodeView.hpp"
#include "Query.hpp"
#include "SampleVector.hpp"

#include <utility>
#include <iostream>
#include <algorithm>
#include <parallel/algorithm>
#include "../io/IO.hpp"

namespace monya {
    namespace container {

    NodeView::NodeView() : depth(0) {
        req_indxs.resize(0);
    }

    NodeView::NodeView(data_t val): NodeView() {
        comparator = val;
    }

    NodeView::NodeView(IndexVector& data_index): NodeView() {
        this->data_index = data_index;
    }

    // Querying
    void NodeView::query(const QueryParams& params, Query* nq) {
        throw not_implemented_exception(__FILE__, __LINE__);
    }

    // Range index
    void NodeView::set_index_range(sample_id_t start_idx,
            const sample_id_t stop_idx) {
        for (sample_id_t idx = start_idx; idx < stop_idx; idx++) {
            req_indxs.push_back(idx);
        }
    }

    // Iterative index
    void NodeView::set_index(const std::vector<sample_id_t>& indxs) {
        req_indxs = indxs; // TODO: Verify copy
    }

    void NodeView::set_index(const sample_id_t* indxs, const size_t nelem) {
        req_indxs.insert(req_indxs.begin(), indxs, indxs+nelem);
    }

    void NodeView::set_index(const sample_id_t index) {
        if (req_indxs.size())
            req_indxs.clear();

        req_indxs.push_back(index);
    }

    // For data index
    void NodeView::set_ph_data_index(const std::vector<sample_id_t>& indxs) {
        data_index.set_indexes(&indxs[0], indxs.size());
    }

    void NodeView::set_ph_data_index(const sample_id_t* indxs,
            const size_t nelem) {
        data_index.set_indexes(indxs, nelem);
    }

    void NodeView::data_index_append(const sample_id_t idx,
            const data_t val) {
        data_index.append(idx, val);
    }
    // End for data index

    void NodeView::set_ioer(io::IO* ioer) {
        this->ioer = ioer;
    }

    io::IO* NodeView::get_ioer() {
        assert(NULL != ioer);
        return ioer;
    }

    void NodeView::set_depth(depth_t depth) {
        this->depth = depth;
    }

    const depth_t NodeView::get_depth() const {
        return depth;
    }

    void NodeView::prep() {
        assert(req_indxs.size() == 1); // TODO: Impl
        data_t* ret = ioer->get_col(req_indxs[0]);
        // Check state of data_index to see if placeholder indexes are there
        if (data_index.empty()) {
            data_index.set_indexes(ret, ioer->shape().first);
        } else {
            // TODO: Bad access pattern for ret vector..
            for (size_t i = 0; i < data_index.size(); i++) {
                auto idx = data_index[i].get_index();
                data_index[i].set_val(ret[idx]);
            }
        }

        if (ioer->get_orientation() == mat_orient_t::ROW)
            delete [] ret;
#if 0
        std::cout << "Printing full index after set:\n";
        data_index.print();
#endif

    }

    void NodeView::sort_data_index(bool par) {
        if (par)
            __gnu_parallel::sort(data_index.begin(),
                    data_index.end());
        else
            std::sort(data_index.begin(), data_index.end());
    }

    IndexVector& NodeView::get_data_index() {
        return data_index;
    }

    void NodeView::print() {
        std::cout << comparator << std::endl;
    }

    const std::string NodeView::to_string() {
        return std::to_string(this->comparator);
    }

    const data_t NodeView::get_comparator() const {
        return comparator;
    }

    void NodeView::set_comparator(const data_t comparator) {
        this->comparator = comparator;
    }

    bool NodeView::operator==(const NodeView& other) {
        return comparator == other.get_comparator();
    }

    bool NodeView::operator!=(const NodeView& other) {
        return !(*this == other);
    }

    bool NodeView::operator<(const NodeView& other) {
        return comparator < other.get_comparator();
    }

    bool NodeView::operator>(const NodeView& other) {
        return get_comparator() > other.get_comparator();
    }

    bool NodeView::operator<=(const NodeView& other) {
        return !(*this > other);
    }

    bool NodeView::operator>=(const NodeView& other) {
        return !(*this < other);
    }

    bool NodeView::is_root() {
        return depth == 0;
    }

    void NodeView::spawn() {
    }

    // @param node: inherits properties from the object
     void NodeView::bestow(NodeView* node) {
         node->parent = this;
         node->set_ioer(ioer);
         node->set_depth(depth+1);

         assert(node->parent->get_comparator() == get_comparator());
         assert(NULL != node->parent);
         assert(NULL != node->get_ioer());
         assert(node->get_depth() > 0); // Root cannot be bestowed
     }

    NodeView::~NodeView() {
    }

#if 0
    std::ostream& operator<<
        (std::ostream& stream, const NodeView& node) {
            stream << node.comparator;
            return stream;
        }
#endif

} } // End monya::container
