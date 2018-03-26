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

namespace monya { namespace container {

    NodeView::NodeView() {
        depth = 0;
        req_indxs.resize(0);
    }

    NodeView::NodeView(data_t val): NodeView() {
        comparator = val;
    }

    NodeView::NodeView(IndexVector<data_t>& data_index): NodeView() {
        this->data_index = data_index;
    }

    // Range index
    void NodeView::set_index_range(sample_id_t start_idx,
            const sample_id_t nsamples) {
        for (sample_id_t idx = start_idx; idx < start_idx; idx++)
            req_indxs.push_back(idx);
    }

    // Iterative index
    void NodeView::set_index(const std::vector<sample_id_t>& indexes) {
        req_indxs = indexes; // TODO: Verify copy
    }

    void NodeView::set_index(const sample_id_t* indexes, const size_t nelem) {
        req_indxs.insert(req_indxs.begin(), indexes, indexes+nelem);
    }

    void NodeView::set_depth(short depth) {
        this->depth = depth;
    }

    const short NodeView::get_depth() const {
        return depth;
    }

    void NodeView::get_data() {
        // TODO
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

    const IndexVector<data_t>& NodeView::get_data_index() const {
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
