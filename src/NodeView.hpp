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

namespace monya { namespace container {

// Represent a node in the tree
template <typename T>
class NodeView {

    public:
        typedef std::shared_ptr<NodeView<T> > ptr;

    protected:
        // Trees either are binary or n-ary -- repr both
        typedef std::pair<NodeView<T>::ptr, NodeView<T>::ptr> Nodepair;
        typedef NodeView<T>::ptr* Nodelist;

        // Represent children of a node
        union Children {
            Nodepair np; // Node pair
            Nodelist nl; // Node list
        };

        NodeView(Children childs=(nullptr, nullptr)) {
            // TODO
        }

        Children childs; // It's english -- just add an s
};

} } // End monya::container
#endif
