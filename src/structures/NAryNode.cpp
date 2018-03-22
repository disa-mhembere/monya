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

// Represent an nary node

#include "NAryNode.hpp"

namespace monya { namespace container {

         NAryNode::NAryNode () {
            this->nchild = 0;
        }

        NAryNode::NAryNode (NodeView** childs, child_t nchild,
                child_t container_size) {
            /**
              * \param container_size: the length of the array containing children
              */
            this->childs = childs;
            this->nchild = nchild;
        }

        void  NAryNode::add_child(NodeView* child) {
            // TODO
        }

        void  NAryNode::remove_child() {
            // TODO
        }
} } // End monya::container
