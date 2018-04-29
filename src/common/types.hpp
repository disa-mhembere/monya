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

#define INVALID_ID -1

namespace monya {
    typedef unsigned node_id_t; // Tree node identifier type
    typedef unsigned sample_id_t; // Tree node identifier type
    typedef unsigned child_t; // The number of children an NAry node can hold
    typedef size_t offset_t; // The offset position of a file
    typedef unsigned tree_t; // The number of trees in the forrest
    typedef size_t depth_t; // The number of trees in the forrest
    typedef double data_t; // The type of the data

    // Used to express the dimensions of a matrix
    typedef std::pair<offset_t, offset_t> dimpair;

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
    //std::map<std::string, short> IOTYPE_t =
        //{{"mem", 0}, {"sem", 1}, {"sync", 2}};

    enum IOTYPE {
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
    enum MAT_ORIENT {
        ROW,
        COL,
        BAND,
        INVALID
    };

    class Params {
        public:
            size_t nsamples; // Max # of samples from which the tree is built
            size_t nfeatures; // Number of features
            std::string fn;
            IOTYPE iotype;
            tree_t ntree;
            unsigned nthread;
            MAT_ORIENT orientation;
            unsigned fanout; // The number of children a node a can have
            size_t max_depth; // Maximum depth the tree can reach

        Params(size_t nsamples=0, size_t nfeatures=0, std::string fn="",
                IOTYPE iotype=IOTYPE::MEM, tree_t ntree=1, unsigned nthread=1,
                MAT_ORIENT orientation=MAT_ORIENT::COL, unsigned fanout=2,
                size_t max_depth=std::numeric_limits<size_t>::max()) {

            this->nsamples = nsamples;
            this->nfeatures = nfeatures;
            this->fn = fn;
            this->iotype = iotype;
            this->ntree = ntree;
            this->nthread = nthread;
            this->orientation = orientation;
            this->fanout = fanout;
            this->max_depth = max_depth;
        }

        const void print () const {
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
                "max depth: " << max_depth << std::endl;
        }
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

            const sample_id_t get_index() const {
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

            const void print() const {
                std::cout << "(" << get_index() << ", " <<
                get_val() << ")" << "\n";
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
        std::vector<IndexVal<data_t> >_;

        public:
        typedef typename std::vector<IndexVal<data_t> >::iterator iterator;
        IndexVector() { } // Default ctor

        IndexVector(const std::vector<data_t>& vals) {
            for (size_t i = 0; i < vals.size(); i++)
                _.push_back(IndexVal<data_t>(i, vals[i]));
        }

        IndexVector(const std::vector<sample_id_t>& v) {
            for (auto idx : v)
                _.push_back(IndexVal<data_t>(idx, 0)); // 0 is a place holder
        }

        IndexVal<data_t>& operator[](const int index) {
            return this->_[index];
        }

        const void print() const {
            for (size_t i = 0; i < _.size(); i++) {
                _[i].print();
            }
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

        const size_t size() const {
            return _.size();
        }

        bool empty() const { return _.empty(); }
        iterator begin() { return _.begin(); }
        iterator end() { return _.end(); }
    };
} // End monya

#endif
