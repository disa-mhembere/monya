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

#ifndef MONYA_TYPES_HPP__
#define MONYA_TYPES_HPP__

// Represent a binary node

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <limits>
#include <cassert>
#include <algorithm>

#include "exception.hpp"

#define INVALID_ID -1

namespace monya {
    typedef unsigned node_id_t; // Tree node identifier type
    typedef unsigned sample_id_t; // Tree node identifier type
    typedef unsigned child_t; // The number of children an NAry node can hold
    typedef size_t offset_t; // The offset position of a file
    typedef unsigned tree_t; // The number of trees in the forest
    typedef size_t depth_t; // The depth of trees in the forest
    typedef float data_t; // The type of the data

    // Constants
    const static depth_t MAX_DEPTH = std::numeric_limits<depth_t>::max();

    // Used to express the dimensions of a matrix
    typedef std::pair<offset_t, offset_t> dimpair;

    // Used for nearest neighbor (NN) queries
    //  Returned results contain the ID of the NN and distance
    typedef std::pair<sample_id_t, data_t> NNpair;

    enum file_t {
        BIN,
        FVECS,
        IVECS,
        BVECS,
        HDF5
    };

    // Traversal order of the tree
    enum order_t {
        PREORDER,
        INORDER,
        POSTORDER,
        LEVELORDER
    };

    enum bchild_t {
        LEFT,
        RIGHT
    };

    /**
      Whether we are running in memory, semi-external memory or in memory
      with synchronous I/O
      */
    enum io_t {
        MEM = 0,
        SEM = 1, // Always async
        SYNC = 2, // Synchronous is for testing only
    };

    /**
       Matrix orientation can be
         ROW: rowwise
         COL: colwise
         BAND: banded
    */
    enum mat_orient_t {
        ROW,
        COL,
        BAND,
        INVALID
    };

    struct Params {
        public:
            size_t nsamples; // Max # of samples from which the tree is built
            size_t nfeatures; // Number of features
            std::string fn;
            io_t iotype;
            tree_t ntree;
            unsigned nthread;
            mat_orient_t orientation;
            unsigned fanout; // The number of children a node a can have
            depth_t max_depth; // Maximum depth the tree can reach
            file_t filetype; // file format

        Params(size_t nsamples=0, size_t nfeatures=0, std::string fn="",
                io_t iotype=io_t::MEM, tree_t ntree=1, unsigned nthread=1,
                mat_orient_t orientation=mat_orient_t::COL, unsigned fanout=2,
                depth_t max_depth=std::numeric_limits<depth_t>::max(),
                file_t filetype=file_t::BIN) {

            this->nsamples = nsamples;
            this->nfeatures = nfeatures;
            this->fn = fn;
            this->iotype = iotype;
            this->ntree = ntree;
            this->nthread = nthread;
            this->orientation = orientation;
            this->fanout = fanout;
            this->max_depth = max_depth;
            this->filetype = filetype;

            if (iotype != io_t::MEM && nthread > 1)
                throw parameter_exception("Multithreading only support for in"
                        " memory mode currently!\n");
        }

        void print() {
            std::cout << "Params: \n" <<
                "# features: " << nfeatures << std::endl <<
                "# samples: "<< nsamples << std::endl <<
                "fn: " << fn << std::endl <<
                "iotype: " << (iotype == MEM ? "Memory" :
                        iotype == SEM ? "Semi-External Memory" :
                        "Synchronous") << std::endl <<
                "# trees " << ntree << std::endl <<
                "# threads " << nthread << std::endl <<
                "orientation: " << (orientation == ROW ? "Row" :
                        orientation == COL ? "Column" :
                        orientation == BAND ? "Band" : "Invalid") << std::endl <<
                "fanout: " << fanout << std::endl <<
                "max depth: " << max_depth << std::endl <<
                "filetype:" << (filetype == BIN ? "Binary" :
                        filetype == FVECS ? "fvecs" :
                        filetype == IVECS ? "ivecs" :
                        filetype == BVECS ? "bvecs": "hdf5") << std::endl;
        }
    };

    struct QueryParams {


    };

    template <typename T>
    class IndexVal {
        private:
            sample_id_t index;
            T val;
        public:
            IndexVal() {
                index = 0;
            }

            IndexVal(const sample_id_t index, const T val) {
                set(index, val);
            }

            void set(const sample_id_t index, const T val) {
                set_index(index);
                set_val(val);
            }

            void set_index(const sample_id_t index) {
                this->index = index;
            }

            const sample_id_t& get_index() const {
                return this->index;
            }

            void set_val(const T val) {
                this->val = val;
            }

            const T& get_val() const {
                return val;
            }

            bool operator< (const IndexVal& other) const {
                return val < other.get_val();
            }

            // Equivalence is only based on the value
            bool operator== (const IndexVal& other) const {
                return val == other.get_val();
            }

            bool operator!= (const IndexVal& other) const {
                return val != other.get_val();
            }

            std::string to_string() {
                return std::string("(") + std::to_string(get_index()) +
                    std::string(", ") + std::to_string(get_val()) +
                    std::string(")");
            }

            void print() {
               printf("%s\n", to_string().c_str());
            }
    };

    template <typename T>
    std::ostream& operator<<
        (std::ostream& stream, const IndexVal<T>& iv) {
            stream << iv.get_index() << " " <<
                iv.get_val() << "\n";
            return stream;
        }

    class IndexVector {
        private:
            std::vector<IndexVal<data_t> >_;
            bool sorted;

        public:
            typedef typename std::vector<IndexVal<data_t> >::iterator iterator;
            IndexVector() : sorted(false) { } // Default ctor

            IndexVector(const size_t nelem) : IndexVector() {
                resize(nelem);
            }

            IndexVector(const std::vector<data_t>& vals) : IndexVector() {
                for (size_t i = 0; i < vals.size(); i++)
                    _.push_back(IndexVal<data_t>(i, vals[i]));
            }

            IndexVector(const std::vector<sample_id_t>& v) : IndexVector() {
                for (auto idx : v)
                    _.push_back(IndexVal<data_t>(idx, 0)); // 0 is a place holder
            }

            IndexVal<data_t>& operator[](const int index) {
                return this->_[index];
            }

            void print() {
                for (size_t i = 0; i < _.size(); i++)
                    _[i].print();
            }

            std::string to_string() {
                std::string __repr__ = "";
                for (auto item : _)
                    __repr__ + item.to_string() + std::string("\n");
                return __repr__;
            }

            // Insert indexes with placeholder values
            void set_indexes(const sample_id_t* idxs, const size_t nelem) {
                if (_.size())
                    _.clear();

                for (size_t i = 0; i < nelem; i++)
                    _.push_back(IndexVal<data_t>(idxs[i], 0));
            }

            // Insert values with contiguous indexes
            void set_indexes(const data_t* vals, const size_t nelem) {
                if (_.size())
                    _.clear();

                for (size_t i = 0; i < nelem; i++)
                    _.push_back(IndexVal<data_t>(i, vals[i]));
            }

            void get_indexes(std::vector<sample_id_t>& v) {
                assert(v.empty());

                for (auto i = begin(); i != end(); ++i) {
                    v.push_back(i->get_index());
                }
            }

            void append(const sample_id_t id, const data_t val) {
                _.push_back(IndexVal<data_t>(id, val));
            }

            void append(IndexVal<data_t>& iv) {
                _.push_back(iv);
            }

            const size_t size() const {
                return _.size();
            }

            void resize(const size_t nelem) {
                assert(nelem >= size());

                for (size_t i = 0; i < nelem-size(); i++) {
                    _.push_back(IndexVal<data_t>(0, 0));
                }
            }

            void insert(IndexVal<data_t> item, const size_t offset) {
                _.insert(begin()+offset, item);
            }

            void trim(const size_t nelem) {
                _.resize(nelem);
            }

            bool empty() const { return _.empty(); }
            iterator begin() { return _.begin(); }
            iterator end() { return _.end(); }

            bool find(IndexVal<data_t>& iv) {
                if (!is_sorted())
                    sort();
                return std::binary_search(begin(), end(), iv);
            }

            void sort() {
                std::sort(begin(), end());
                sorted = true;
            }

            void set_sorted(const bool sorted) {
                this->sorted = sorted;
            }

            const bool is_sorted() const {
                return sorted;
            }
    };


    // circular integer
    class cunsigned {
        private:
            unsigned val;
            unsigned max; // NOTE: Exclusive so `max` never occurs i.e like mod

        public:
            cunsigned(unsigned max) : val(0), max(max) {
                if (max == 0)
                    throw parameter_exception("max must be > 0",
                            __FILE__, __LINE__);
            }

            cunsigned(unsigned val, unsigned max) : val(val), max(max) {
                if (max == 0)
                    throw parameter_exception("max must be > 0",
                            __FILE__, __LINE__);
                if (val >= max)
                    throw parameter_exception("val must be < max",
                            __FILE__, __LINE__);
            }

            void set(unsigned val) {
                this->val = val;
                if (val >= max)
                    throw parameter_exception("val must be < max",
                            __FILE__, __LINE__);
            }

            unsigned inc() {
                val = val == max - 1 ? 0 : val + 1;
                return val;
            }

             unsigned dec() {
                val = val == 0 ? max - 1: val - 1;
                return val;
            }

            const unsigned get() const { return val; }
    };
} // End monya

#endif
