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

#ifndef NODEVIEW_HPP__
#define NODEVIEW_HPP__

#include <memory>
#include <utility>

#include "../common/types.hpp"

namespace monya {
    namespace container {

class TreeHeader {
    public:
        node_id_t nnodes; // Total number of nodes in a tree
        short dtype; // Size (Bytes) of data stored at node
};

class BinaryTreeHeader: public TreeHeader {
    public:
        node_id_t nnodes;
        node_id_t nchilds; // How many children each node has

    BinaryTreeHeader() {
        this->nchilds = 2;
    }
};

class NodeHeader {
    public:
        node_id_t id;

        NodeHeader() {
            this->id = INVALID_ID;
        }

        NodeHeader(node_id_t _id): id(_id) {
        }

        // TODO
};

// For variable length nodes this is per node
class VarNodeHeader: public NodeHeader {

    public:

        node_id_t datasize;
        VarNodeHeader(const node_id_t datasize) {
            this->datasize = datasize;
        }

        // TODO
};

// Represent a node in the tree
template <typename T>
class EMNode {
    // TODO
};

} } // End monya::container
#endif
