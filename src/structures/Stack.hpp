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

#ifndef MONYA_STACK_HPP__
#define MONYA_STACK_HPP__

#include <cstdlib>
#include <vector>

namespace monya { namespace container {
    template <typename DataType>
        class Stack {
            private:
                std::vector<DataType> _;

            public:
                typedef typename std::vector<DataType>::iterator iterator;

                Stack() { }

                void push(DataType elem) {
                    _.push_back(elem);
                }

                DataType pop() {
                    DataType back = _.back();
                    _.pop_back();
                    return back;
                }

                const size_t size() const {
                    return _.size();
                }

                bool empty() const { return _.empty(); }
                iterator begin() { return _.begin(); }
                iterator end() { return _.end(); }
        };
}}

#endif
