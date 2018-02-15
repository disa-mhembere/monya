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
template <typename DataType>
class RBNode : public BinaryNode<DataType> {
    protected:

    public:
        // TODO: Visibility
        uint8_t color; //0 = B, 1 = R

        // Couldn't figure out a way out of this variable redundancy
        RBNode<DataType>* parent;
        RBNode<DataType>* left;
        RBNode<DataType>* right;

        using BinaryNode<DataType>::BinaryNode;
        //RBNode(): BinaryNode<DataType>() {
        //}

        //RBNode(DataType val): BinaryNode<DataType>(val) {
        //}

        //RBNode(std::vector<offset_t>& data_index):
            //BinaryNode<DataType>(data_index) {
        //}
};

}} // End namespace monya::container
#endif
