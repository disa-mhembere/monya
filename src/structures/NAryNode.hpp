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

#ifndef MONYA_NARY_NODE_HPP__
#define MONYA_NARY_NODE_HPP__

// Represent an nary node

#include "NodeView.hpp"

namespace monya { namespace container {

class NAryNode: public NodeView {

    private:
        child_t nchild;
        NodeView** childs;

        void resize_child_container(size_t to) {
            // TODO
        }

        void grow_container() {
            // TODO
        }

        void shrink_container() {
            // TODO
        }

    public:

        NAryNode ();
        NAryNode (NodeView** childs, child_t nchild,
                child_t container_size);
        void add_child(NodeView* child);
        void remove_child();
        static NAryNode* cast2(NodeView* nv);

        virtual void prep() override { /*TODO*/ };
        virtual void run() override { /*TODO*/ };
        virtual const bool has_child() override { return NULL != childs; }
};
} } // End monya::container

#endif
