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

#ifndef RB_TREE_HPP__
#define RB_TREE_HPP__

#include <memory>
#include "../common/types.hpp"
#include "../common/exception.hpp"

namespace monya { namespace container {

template <typename NodeType>
class RBTree {
    private:
        enum Color {
            RED,
            BLACK
        };

        struct Node {
            Color color;
            NodeType key;
            Node *parent;
            Node *left;
            Node *right;
        };

        Node *root;

        void rotate_left(Node *x) {
            Node *y;

            y = x->right;
            x->right = y->left;

            if (y->left) {
                y->left->parent = x;
            }

            y->parent = x->parent;
            y->left = x;

            if (!x->parent) {
                root = y;
            } else if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
            x->parent = y;
        }

        void rotate_right(Node *y) {
            Node *x;

            x = y->left;
            y->left = x->right;
            if (x->right) {
                x->right->parent = y;
            }

            x->parent = y->parent;
            x->right = y;

            if (!y->parent) {
                root = x;
            } else if (y == y->parent->left) {
                y->parent->left = x;
            } else {
                y->parent->right = x;
            }

            y->parent = x;
        }

        void transplant(Node *dest, Node *src) {
            if (dest->parent == NULL) {
                root = src;
            } else if (dest == dest->parent->left) {
                dest->parent->left = src;
            } else {
                dest->parent->right = src;
            }

            if (src) {
                src->parent = dest->parent;
            }
        }

        Node *minimum(Node *tree) {
            while (tree->left) {
                tree = tree->left;
            }

            return tree;
        }

        void echo(Node *node, int tabs) {
            if (!node) {
                return;
            }

            echo(node->left, tabs + 1);

            for (int i = 0; i < tabs; ++i) {
                std::cout << "\t\t";
            }
            std::cout << *node->key << (node->color ? "B" : "R") << std::endl;

            echo(node->right, tabs + 1);
        }

        void delete_node(Node *node) {
            if (!node) {
                return;
            }

            if (node->left) {
                delete_node(node->left);
            }

            if (node->right) {
                delete_node(node->right);
            }

            delete node;
        }

        RBTree() : root(NULL) {
        }

    public:

        typedef std::shared_ptr<RBTree<NodeType> > ptr;

        static ptr create() {
            return ptr(new RBTree<NodeType>());
        }

        void insert(NodeType key) {
            Node *node, *parent, *z;

            parent = NULL;
            node = root;
            while (node) {
                parent = node;
                if ((*key) < *(node->key)) {
                    node = node->left;
                } else {
                    node = node->right;
                }
            }

            if (!parent) {
                z = root = new Node;
                z->key = key;
                z->color = BLACK;
                z->parent = z->left = z->right = NULL;
            } else {
                z = new Node;
                z->key = key;
                z->color = RED;
                z->parent = parent;
                z->left = z->right = NULL;

                if (*(z->key) < *(parent->key)) {
                    parent->left = z;
                } else {
                    parent->right = z;
                }
            }

            Node *uncle;
            bool side;
            while (z->parent && z->parent->color == RED) {
                if ((side = (z->parent == z->parent->parent->left))) {
                    uncle = z->parent->parent->right;
                } else {
                    uncle = z->parent->parent->left;
                }

                if (uncle && uncle->color == RED) {
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == (side ? z->parent->right : z->parent->left)) {
                        z = z->parent;
                        side ? rotate_left(z) : rotate_right(z);
                    }

                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    side ? rotate_right(z->parent->parent) :
                        rotate_left(z->parent->parent);
                }
            }

            root->color = BLACK;
        }

        NodeType& find(const NodeType& keyshell) {
            Node *node = root;
            while (node) {
                if (*(node->key) > (*keyshell)) {
                    node = node->left;
                } else if (*(node->key) < (*keyshell)) {
                    node = node->right;
                } else {
                    return node->key;
                }
            }

            throw std::runtime_error("NodeType not found");
        }

        void _delete(const NodeType& key) {
            Node *node = root;
            while (node) {
                if (*(node->key) > (*key)) {
                    node = node->left;
                } else if (*(node->key) < (*key)) {
                    node = node->right;
                } else {
                    break;
                }
            }

            if (!node || *(node->key) != (*key)) {
                return;
            }

            Color original;
            Node *sub, *old;
            if (!node->left) {
                transplant(node, sub = node->right);
            } else if (!node->right) {
                transplant(node, sub = node->left);
            } else {
                old = minimum(node->right);
                original = old->color;
                sub = old->right;

                if (old->parent == node) {
                    sub->parent = node;
                } else {
                    transplant(old, old->right);
                    old->right = node->right;
                    old->right->parent = old;
                }

                transplant(node, old);
                old->left = node->left;
                old->left->parent = old;
                old->color = node->color;
            }

            delete node;
            if (original == BLACK) {
                bool side;
                Node *sibling;
                while (old != root && old->color == BLACK) {
                    if ((side = (old == old->parent->left))) {
                        sibling = old->parent->right;
                    } else {
                        sibling = old->parent->left;
                    }

                    if (sibling->color == RED) {
                        sibling->color = BLACK;
                        old->parent->color = RED;
                        side ? rotate_left(old->parent) : rotate_right(old->parent);
                        sibling = side ? old->parent->right : old->parent->left;
                    }

                    if (sibling->left->color == BLACK &&
                            sibling->right->color == RED) {
                        sibling->color = RED;
                        old = old->parent;
                    } else {
                        if (BLACK == side ? sibling->right->color :
                                sibling->left->color) {
                            sibling->color = RED;
                            if (side) {
                                sibling->left->color = BLACK;
                                rotate_right(sibling);
                                sibling = old->parent->right;
                            } else {
                                sibling->right->color = BLACK;
                                rotate_left(sibling);
                                sibling = old->parent->left;
                            }
                        }

                        sibling->color = old->parent->color;
                        old->parent->color = BLACK;
                        if (side) {
                            sibling->left->color = BLACK;
                            rotate_left(old->parent);
                        } else {
                            sibling->right->color = BLACK;
                            rotate_right(old->parent);
                        }

                        old = root;
                    }
                }
            }
        }

        void echo() {
            echo(root, 0);
        }

        ~RBTree() {
            delete_node(root);
        }


};
} } // End monya::container

#endif
