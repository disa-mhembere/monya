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

#include "../common/types.hpp"
#include "../../SAFS/libsafs/io_interface.h"

namespace monya {
    namespace io {
        class IO;
    }

    namespace container {

// Fwd decl
class Scheduler;

// Represent a node in the tree
class NodeView: public safs::callback {
    public:
        virtual void run() = 0;
        virtual void init(Params&) = 0;
        virtual void distance(data_t arg1) = 0;

        virtual void spawn(std::vector<sample_id_t>& idxs,
                std::vector<offset_t>& offsets);

        // TODO: Visibility
        //std::vector<offset_t> data_index; // Indexes that nodes hold to data
        IndexVector<double> data_index; // Indexes that nodes hold to data & mapping

        // FIXME: mem waster
        std::vector<sample_id_t> req_indxs; // Indexes a vertex will req from ioer
        char* buf; // The data read from dataset
        data_t comparator; // The split comparator
        unsigned numbytes; // The number of bytes in the read of data from disk
        // When the data required is in memory run this computation
        short depth; // Depth of the node used as an idendifier
        Scheduler* scheduler;
        io::IO* ioer;

        // TODO: End visibility

        virtual int invoke(safs::io_request *reqs[], int num)
            override {
            for (int i = 0; i < num; i++) {
                this->buf = reqs[i]->get_buf();
                this->numbytes = reqs[i]->get_size();
                free(this->buf); // TODO: Verify OK
            }
            return EXIT_SUCCESS;
        }

        NodeView();
        NodeView(data_t val);
        NodeView(IndexVector<data_t>& data_index);

        void schedule();

        // Range index
        virtual void set_index_range(sample_id_t start_idx,
                const sample_id_t nsamples);

        // Defaults to grabbing index data
        virtual void prep();

        // Iterative index
        void set_index(const std::vector<sample_id_t>& indexes);
        void set_index(const sample_id_t*, const size_t);

        // IO
        void set_ioer(io::IO* ioer);
        typename io::IO* get_ioer();

        void set_depth(short depth);
        const short get_depth() const;
        void get_data();
        void set_scheduler(Scheduler* scheduler);
        Scheduler* get_scheduler();
        void sort_data_index(bool par=false);
        const IndexVector<data_t>& get_data_index() const;
        virtual const void print() const;
        const data_t get_comparator() const;
        void set_comparator(const data_t comparator);
        virtual bool operator==(const NodeView& other);
        virtual bool operator!=(const NodeView& other);
        virtual bool operator<(const NodeView& other);
        virtual bool operator>(const NodeView& other);
        virtual bool operator<=(const NodeView& other);
        virtual bool operator>=(const NodeView& other);
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
