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

#ifndef BINARY_TREE_PARTITION_HPP__
#define BINARY_TREE_PARTITION_HPP__

#include "TreePartition.hpp"

namespace monya { namespace container {

template <typename NodeType>
class BinaryTreePartition : public TreePartition <NodeType> {

    private:
        BinaryTreePartition() {
        }

    public:
        typedef std::shared_ptr<BinaryTreePartition <NodeType> > ptr;

        static ptr create() {
            return TreePartition<NodeType>::ptr(new BinaryTreePartition<NodeType>());
        }

        //static ptr cast2(TreePartition<NodeType>::ptr tp) {
            //return std::static_pointer_cast<BinaryTreePartition<NodeType> >(tp);
        //}
};

} } // End monya::container

#endif
