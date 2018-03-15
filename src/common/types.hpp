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

#define INVALID_ID -1

namespace monya {
    typedef unsigned node_id_t; // Tree node identifier type
    typedef unsigned sample_id_t; // Tree node identifier type
    typedef unsigned child_t; // The number of children an NAry node can hold
    typedef size_t offset_t; // The offset position of a file
    typedef unsigned tree_t; // The number of trees in the forrest
    typedef size_t depth_t; // The number of trees in the forrest

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
    std::map<std::string, short> IOTYPE_t =
        {{"mem", 0}, {"sem", 1}, {"sync", 2}};

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
            short max_depth; // Maximum depth the tree can reach

        Params(size_t nsamples=0, size_t nfeatures=0, std::string fn="",
                IOTYPE iotype=IOTYPE::MEM, tree_t ntree=1, unsigned nthread=1,
                MAT_ORIENT orientation=MAT_ORIENT::COL, unsigned fanout=2,
                short max_depth=std::numeric_limits<short>::max()) {

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
    };

    std::ostream& operator<<
        (std::ostream& stream, const Params& params) {
            stream << "Params: \n" <<
            "# features: " << params.nfeatures << std::endl <<
            "# samples: "<< params.nsamples << std::endl <<
            "fn: " << params.fn << std::endl <<
            "iotype: " << (params.iotype == MEM ? "Memory" :
                params.iotype == SEM ? "Semi-External Memory" :
                "Synchronous") << std::endl <<
            "# trees " << params.ntree << std::endl <<
            "# threads " << params.nthread << std::endl <<
            "orientation: " << (params.orientation == ROW ? "Row" :
                params.orientation == COL ? "Column" :
                params.orientation == BAND ? "Band" : "Invalid") << std::endl <<
            "fanout: " << params.fanout << std::endl <<
            "max depth: " << params.max_depth << std::endl;

            return stream;
        }

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
    };

    template <typename T>
    class IndexVector {
        std::vector<IndexVal<T> > _;

        public:
        IndexVector() { } // Default ctor

        IndexVector(const std::vector<T>& vals) {
            for (size_t i = 0; i < vals.size(); i++)
                _.push_back(IndexVal<T>(i, vals[i]));
        }

        IndexVal<T>& operator[](const int index) {
            return this->_[index];
        }

        void print() {
            for (size_t i = 0; i < _.size(); i++)
                std::cout << "Index: " <<  _[i].get_index() <<
                    ", Val: " << _[i].get_val() << std::endl;

        }
    };
} // End monya

#endif
