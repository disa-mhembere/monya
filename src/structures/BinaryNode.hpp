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

#ifndef BINARY_NODE_HPP__
#define BINARY_NODE_HPP__

// Represent a binary node

#include "NodeView.hpp"
#include <iostream>
#include "io_interface.h"

namespace monya { namespace container {

template <typename T>
class BinaryNode: public NodeView<T>, public safs::callback {

    private:
        T data; // Node data
        char* buf; // TODO templatize -- the data read from disk
        int numbytes; // The number of bytes in the read of data from disk

    public:

        virtual int invoke(safs::io_request *reqs[], int num) override {
            for (int i = 0; i < num; i++) {
                this->buf = reqs[i]->get_buf();
                this->numbytes = reqs[i]->get_size();
                free(this->buf); // TODO: Verify OK
            }
            return EXIT_SUCCESS;
        }

        BinaryNode (T data, NodeView<T>* left=NULL, NodeView<T>* right=NULL) {
            this->data = data;
            this->left;
            this->right;
        }

        void set_data(T data) {
            this->data = data;
        }

        void set_left(NodeView<T>* left) {
            this->left = left;
        }

        void set_right(NodeView<T>* right) {
            this->right = right;
        }

        T get_data() {
            return data;
        }

        static BinaryNode<T>* cast2(NodeView<T>* nv) {
            return static_cast<BinaryNode<T> >(nv);
        }

        void print() {
            std::cout << data << std::endl;
        }

        void read_svm() {
        }

        // What computation does the user want to run when data hits memory
        void run() override {
            // TODO
        }

        // Does the user want to perm I/O if so then pick the data to read
        void prep() override {
            // TODO
        }

        // Async writeback
        void persist() {
            // TODO
        }

        // Keep the node in memory
        void cache() {
            // TODO
        }

        // Does this node want to bear children?
        void spawn() {
        }
};
} } // End monya::container

#endif
