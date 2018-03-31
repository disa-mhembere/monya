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

#include "RBTree.hpp"
#include "../common/exception.hpp"
#include "RBNode.hpp"

namespace monya { namespace container {

    void RBTree::rotate_left(RBNode* x) {
        RBNode* y;

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

    void RBTree::rotate_right(RBNode* y) {
        RBNode* x;

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

    void RBTree::transplant(RBNode* dest, RBNode* src) {
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

    RBNode* RBTree::minimum(RBNode* tree) {
        while (tree->left) {
            tree = tree->left;
        }

        return tree;
    }

    void RBTree::echo(RBNode* node, int tabs) {
        if (!node) {
            return;
        }

        echo(node->left, tabs + 1);

        for (int i = 0; i < tabs; ++i) {
            std::cout << "\t\t";
        }
        std::cout << node->comparator <<
            (node->color ? "B" : "R") << std::endl;

        echo(node->right, tabs + 1);
    }

    void RBTree::delete_node(RBNode* node) {
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

    void RBTree::set_root(RBNode* node) {
        assert(NULL != node);
        insert(node);
    }

    size_t RBTree::get_nnodes(RBNode* node, size_t& nnodes) {
        if (!node) {
            return nnodes;
        }

        nnodes++;
        get_nnodes(node->left, nnodes);
        get_nnodes(node->right, nnodes);
        return nnodes;
    }

    void RBTree::insert_at(RBNode* new_node,
            RBNode* node, bchild_t pos) {
        if (NULL != node->left)
            throw std::runtime_error("Left child already populated");

        RBNode* parent = node;

        // If the parent is NULL then the tree is empty
        //  Root is always black
        if (!parent) {
            new_node = root = node;
            new_node->color = 0;
            new_node->parent = new_node->left = new_node->right = NULL;
            depth = 1; // Data race ok
        } else {
            new_node = node;
            new_node->color = 1;
            new_node->parent = parent;
            new_node->left = new_node->right = NULL;

            if (pos == bchild_t::LEFT)
                parent->left = new_node;
            else
                parent->right = new_node;
        }

        balance(parent, node, new_node);
    }

    void RBTree::insert(RBNode* node) {
        RBNode* tmp; // A tmp node used to find the parent
        RBNode* parent; // Will hold the parent of node being inserted
        RBNode* new_node = NULL; // The new node being inserted

        parent = NULL;
        tmp = root;
        // Traverse the tree to find the parent of the node to be inserted
        while (tmp) {
            parent = tmp;
            if (*node < *tmp) {
                tmp = tmp->left;
            } else {
                tmp = tmp->right;
            }
        }

        // If the parent is NULL then the tree is empty
        //  Root is always black
        if (!parent) {
            new_node = root = node;
            new_node->color = 0;
            new_node->parent = new_node->left = new_node->right = NULL;
        } else {
            new_node = node;
            new_node->color = 1;
            new_node->parent = parent;
            new_node->left = new_node->right = NULL;

            if (*(new_node) < *(parent)) {
                parent->left = new_node;
            } else {
                parent->right = new_node;
            }
        }

        balance(parent, node, new_node);
    }

    void RBTree::balance(RBNode* parent,
            RBNode* node, RBNode* new_node) {
        // If the parent is NULL then the tree is empty
        //  Root is always black
        if (!parent) {
            new_node = root = node;
            new_node->color = 0;
            new_node->parent = new_node->left = new_node->right = NULL;
        } else {
            new_node = node;
            new_node->color = 1;
            new_node->parent = parent;
            new_node->left = new_node->right = NULL;

            if (*(new_node) < *(parent)) {
                parent->left = new_node;
            } else {
                parent->right = new_node;
            }
        }

        RBNode *uncle;
        bool side;
        while (new_node->parent && new_node->parent->color == 1) {
            if ((side = (new_node->parent ==
                            new_node->parent->parent->left))) {
                uncle = new_node->parent->parent->right;
            } else {
                uncle = new_node->parent->parent->left;
            }

            if (uncle && uncle->color == 1) {
                new_node->parent->color = 0;
                uncle->color = 0;
                new_node->parent->parent->color = 1;
                new_node = new_node->parent->parent;
            } else {
                if (new_node == (side ? new_node->parent->right :
                            new_node->parent->left)) {
                    new_node = new_node->parent;
                    side ? rotate_left(new_node) : rotate_right(new_node);
                }

                new_node->parent->color = 0;
                new_node->parent->parent->color = 1;
                side ? rotate_right(new_node->parent->parent) :
                    rotate_left(new_node->parent->parent);
            }
        }
        root->color = 0;
    }

    RBNode* RBTree::find(const RBNode* shell) {
        RBNode *node = root;
        while (node) {
            if (*node > *shell) {
                node = node->left;
            } else if (*node < *shell) {
                node = node->right;
            } else {
                return node;
            }
        }

        throw std::runtime_error("node not found");
    }

    void RBTree::_delete(const RBNode& shell) {
        RBNode *node = root;
        while (node) {
            if (*node > shell) {
                node = node->left;
            } else if (*node < shell) {
                node = node->right;
            } else {
                break;
            }
        }

        if (!node || *node != shell) {
            return;
        }

        uint8_t original = 0;
        RBNode* sub = NULL;
        RBNode* old = NULL;
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
        if (original == 0) {
            bool side;
            RBNode *sibling;
            while (old != root && old->color == 0) {
                if ((side = (old == old->parent->left))) {
                    sibling = old->parent->right;
                } else {
                    sibling = old->parent->left;
                }

                if (sibling->color == 1) {
                    sibling->color = 0;
                    old->parent->color = 1;
                    side ? rotate_left(old->parent) : rotate_right(old->parent);
                    sibling = side ? old->parent->right : old->parent->left;
                }

                if (sibling->left->color == 0 &&
                        sibling->right->color == 1) {
                    sibling->color = 1;
                    old = old->parent;
                } else {
                    if (0 == side ? sibling->right->color :
                            sibling->left->color) {
                        sibling->color = 1;
                        if (side) {
                            sibling->left->color = 0;
                            rotate_right(sibling);
                            sibling = old->parent->right;
                        } else {
                            sibling->right->color = 0;
                            rotate_left(sibling);
                            sibling = old->parent->left;
                        }
                    }

                    sibling->color = old->parent->color;
                    old->parent->color = 0;
                    if (side) {
                        sibling->left->color = 0;
                        rotate_left(old->parent);
                    } else {
                        sibling->right->color = 0;
                        rotate_right(old->parent);
                    }

                    old = root;
                }
            }
        }
    }

    void RBTree::echo() {
        echo(root, 0);
    }

    RBTree::~RBTree() {
        delete_node(root);
    }
} } // End monya::container
