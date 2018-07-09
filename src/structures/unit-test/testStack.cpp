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

#include "Stack.hpp"
#include <cassert>

using namespace monya::container;

int main() {
    Stack<size_t> s;

    // Push 10 elements
    for (size_t i = 0; i < 10; i++) {
        s.push(i);
    }

    // Check 5 elements
    for (size_t i = 9; i > 4; i--) {
        assert(s.pop() == i);
        assert(s.size() == i);
    }

    // Pop it all
    int i = 0;
    while (!s.empty()) {
        assert(s.pop() < 5);
        i++;
    }
    assert(i == 5);

    assert(!(bool)s.size() == s.empty());
    return EXIT_SUCCESS;
}
