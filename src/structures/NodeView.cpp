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
#include "Scheduler.hpp"

#include <utility>
#include <iostream>
#include <algorithm>
#include <parallel/algorithm>
#include "../io/IO.hpp"

namespace monya { namespace container {

    NodeView::NodeView() {
        depth = 0;
        req_indxs.resize(0);
    }

    const bool NodeView::is_leaf() const {
        return (depth == get_max_depth() || get_data_index().size() == 1);
    }

    NodeView::NodeView(data_t val): NodeView() {
        comparator = val;
    }

    NodeView::NodeView(IndexVector& data_index): NodeView() {
        this->data_index = data_index;
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

    const depth_t NodeView::get_max_depth() const {
        return scheduler->get_max_depth();
    }

    // This is run first
    void NodeView::prep() {
        if (get_depth() <= get_max_depth())
            get_data(); // Put data into mem
    }

    void NodeView::get_data() {
        assert(ioer->get_orientation() == MAT_ORIENT::COL); // TODO: Impl
        // FIXME
        assert(req_indxs.size() == 1); // TODO: Impl
        data_t* ret = ioer->get_col(req_indxs[0]);

        // Check state of data_index to see if placeholder indexes are there
        if (data_index.empty()) {
            data_index.set_indexes(ret, ioer->shape().first);
        } else {
            // FIXME works with 1 column only
            for (auto iv : data_index) {
                iv.set_val(ret[iv.get_index()]);
            }
        }
    }

    void NodeView::set_scheduler(Scheduler* scheduler) {
        this->scheduler = scheduler;
    }

    Scheduler* NodeView::get_scheduler() {
        return this->scheduler;
    }

    void NodeView::sort_data_index(bool par) {
        if (par)
            __gnu_parallel::sort(data_index.begin(),
                    data_index.end());
        else
            std::sort(data_index.begin(), data_index.end());
    }

    const IndexVector& NodeView::get_data_index() const {
        return data_index;
    }

    const void NodeView::print() const {
        std::cout << comparator << std::endl;
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
        return this->get_comparator() > other.get_comparator();
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

    void NodeView::spawn(std::vector<sample_id_t>& idxs,
            std::vector<offset_t>& offsets) {
    }

    void NodeView::schedule() {
        scheduler->schedule(this);
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
