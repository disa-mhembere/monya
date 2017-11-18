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

#ifndef NODE_HPP__
#define NODE_HPP__

// Represent a node

#include "NodeView.hpp"

namespace monya { namespace container {

template <typename T>
class Node: public NodeView <T> {

    public:
        std::shared_ptr<Node<T> > cast2(typename NodeView<T>::ptr nv) {
            return std::static_pointer_cast<Node<T> >(nv);
        }

        static typename NodeView<T>::ptr create(typename NodeView<T>::ptr left=nullptr,
                typename NodeView<T>::ptr right=nullptr) {
            return ptr(new NodeView<T>(left, right));
        }
};
} } // End monya::container

#endif
