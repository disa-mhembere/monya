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

#ifndef MONYA_RB_NODE_HPP__
#define MONYA_RB_NODE_HPP__

#include "BinaryNode.hpp"

namespace monya { namespace container {
class RBNode : public BinaryNode {
    public:
        // TODO: Visibility
        uint8_t color; // 0 = B, 1 = R

        RBNode* parent;
        RBNode* left;
        RBNode* right;

        using BinaryNode::BinaryNode;

        static RBNode* cast2(BinaryNode* node) {
            return static_cast<RBNode*>(node);
        }
};

}} // End namespace monya::container
#endif
