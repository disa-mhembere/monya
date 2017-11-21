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

namespace monya {
    typedef unsigned child_t; // The number of children an NAry node can hold

    namespace container {

// Represent a node in the tree
template <typename T>
class NodeView {

    protected:
        // For nary trees we represent children as a list
        // TODO: optimize for large fanout
        typedef NodeView<T>** NodeList;

        // For Binary Trees we only need nodes to have two children
        class NodePair {
            public:
                NodeView<T>* left;
                NodeView<T>* right;
                NodePair(NodeView<T>* left, NodeView<T>* right) {
                    this->left = left;
                    this->right = right;
                }
        };

        // Represent children of a node
        union Children {
            NodePair np; // Node pair
            NodeList nl; // Node list
        };

        NodeView(NodeView<T>* left, NodeView<T>* right) {
            childs->left = left;
            childs->right = right;
        }

        Children childs; // It's english -- just add an s
};

} } // End monya::container
#endif
