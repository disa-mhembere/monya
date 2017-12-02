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

#ifndef TREE_HPP__
#define TREE_HPP__

#include <memory>
#include "../common/types.hpp"
#include "../common/exception.hpp"
#include "BinaryNode.hpp"

namespace monya { namespace container {

template <typename T>
class Tree {

    private:
        short numa_id;
        NodeView<T>* root;

        Tree(NodeView<T>* root) {
            this->root = NULL;
        }

    public:
        typedef std::shared_ptr<Tree<T> > ptr;

        static ptr create(NodeView<T>* root=NULL) {
            return ptr(new Tree<T>(root));
        }

        void materialize_node(const node_id_t id) {
            // TODO
        }

        void virtualize() {
            // TODO
        }

        // Tester
        void echo() {
            //apply<print_node>
        }


        static void print_node(NodeView<T>* node) {
            node->print();
        }

        // TODO: Optimize
        template<typename CallbackFunction>
        void apply(BinaryNode<T>* node, CallbackFunction&& f,
                order_t traversal=PREORDER) {
            switch (traversal) {
                case PREORDER:
                    if (node->left())
                        apply(node->left(), f, traversal);
                    if (node->right())
                        apply(node->right(), f, traversal);
                    else
                        f(node);
                    break;
                case INORDER:
                    throw monya::not_implemented_exception("INORDER");
                    break;
                case INORDER:
                    throw monya::not_implemented_exception("INORDER");
                    break;
                case INORDER:
                    throw monya::not_implemented_exception("INORDER");
                    break;
                default:
                    throw monya::parameter_exception(std::to_string(traversal));
            }
        }

        ~Tree() {
            std::cout << "deleting nodes\n";
        }
};

} } // End monya::container

#endif
