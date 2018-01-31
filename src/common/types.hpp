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

#define INVALID_ID -1

namespace monya {
    typedef unsigned node_id_t;
    typedef unsigned child_t; // The number of children an NAry node can hold
    typedef size_t offset_t; // The offset position of a file
    typedef unsigned tree_t; // The number of trees in the forrest

    // Used to express the dimensions of a matrix
    typedef std::pair<offset_t, offset_t> dimpair;

    // Traversal order of the tree
    enum order_t {
        PREORDER,
        INORDER,
        POSTORDER,
        LEVELORDER
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
            std::string fn;
            IOTYPE iotype;
            tree_t ntree;

        Params(std::string fn="", IOTYPE iotype=IOTYPE::MEM, tree_t ntree=1) {
            this->fn = fn;
            this->iotype = iotype;
            this->ntree = ntree;
        }
    };
} // End monya

#endif
