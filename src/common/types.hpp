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

#define INVALID_ID -1

namespace monya {
    typedef unsigned node_id_t;
    typedef unsigned child_t; // The number of children an NAry node can hold
    typedef size_t offset_t; // The offset position of a file

    // Traversal order of the tree
    enum order_t {
        PREORDER,
        INORDER,
        POSTORDER,
        LEVELORDER
    };
} // End monya

#endif
