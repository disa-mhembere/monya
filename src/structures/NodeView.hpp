/*
 * Copyright 2017 Neurodata (https://neurodata.io)
 * Written by Disa Mhembere (disa@cs.jhu.edu)
 *
 * This file is part of Monya.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MONYA_NODEVIEW_HPP__
#define MONYA_NODEVIEW_HPP__

#include "../common/types.hpp"
#include "../common/exception.hpp"

#ifdef __unix__
#include "../../SAFS/libsafs/io_interface.h"
#endif

namespace monya {
    namespace io {
        class IO;
    }

    namespace container {

// Fwd decl
class Query;
class SampleVector;

// Represent a node in the tree
#ifdef __unix__
class NodeView: public safs::callback {
#else
class NodeView {
#endif
    protected:
        // TODO: Visibility
        IndexMatrix data_index; // Indexes that nodes hold to data & mapping

        data_t comparator; // The split comparator
        // When the data required is in memory run this computation
        depth_t depth; // Depth of the node used as an idendifier
        io::IO* ioer;
        NodeView* parent;
        // TODO: End visibility

    public:
        data_t& get_comparator() { return comparator; }

        virtual void run() = 0;
        virtual void init(Params&) = 0;
        virtual void request(const sample_id_t index);
        virtual void row_request(const sample_id_t index);

        virtual data_t distance(SampleVector*, const sample_id_t) {
            throw abstract_exception("NodeView::distance");
        }

        virtual data_t distance(data_t*, const sample_id_t) {
            throw abstract_exception("NodeView::distance");
        }

        virtual void query(const QueryParams&, Query*);
        virtual void spawn();

#ifdef __unix__
        virtual int invoke(safs::io_request *reqs[], int num) override {
            for (int i = 0; i < num; i++) {
                char* buf = reqs[i]->get_buf();
                size_t numbytes = reqs[i]->get_size();

                // TODO: Do something with buf and numbytes
                assert(numbytes);
                free(buf);
            }
            return EXIT_SUCCESS;
        }
#endif

        NodeView();
        NodeView(data_t val);
        NodeView(IndexMatrix& data_index);

        virtual const bool has_child() = 0;
        virtual bool can_split();
        void schedule();

        // Defaults to grabbing index data
        virtual void prep();

        // Request/Get Iterative index
        void set_index(const std::vector<sample_id_t>& indexes);
        void set_index(const sample_id_t*, const size_t nelem);
        void set_index(const sample_id_t index);

        // Data_index i.e. samples index
        void set_ph_data_index(const std::vector<sample_id_t>& indexes,
                const size_t pos);
        void set_ph_data_index(const sample_id_t*,
                const size_t nelem, const size_t pos);
        void data_index_append(const sample_id_t idx);
        void data_index_append(const sample_id_t idx, const data_t val=0);

        // IO
        void set_ioer(io::IO* ioer);
        typename io::IO* get_ioer();

        // Params
        void set_depth(depth_t depth);
        const depth_t get_depth() const;
        const depth_t get_max_depth() const;
        void sort_data_index(bool par=false);
        void sort_data_index(bool par, const size_t pos);
        IndexMatrix& get_data_index();
        virtual void print();
        virtual const std::string to_string();
        const data_t get_comparator() const;
        void set_comparator(const data_t comparator);
        virtual void deschedule();
        virtual bool operator==(const NodeView& other);
        virtual bool operator!=(const NodeView& other);
        virtual bool operator<(const NodeView& other);
        virtual bool operator>(const NodeView& other);
        virtual bool operator<=(const NodeView& other);
        virtual bool operator>=(const NodeView& other);

        virtual void bestow(NodeView* node);
        bool is_root();
        virtual ~NodeView();
};

#if 0
std::ostream& operator<<
    (std::ostream& stream, const NodeView& node) {
    stream << node.comparator;
    return stream;
}
#endif

} } // End monya::container
#endif
