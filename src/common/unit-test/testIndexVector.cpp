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

#include "../types.hpp"
#include <random>

#include <cassert>

int main(int argv, char* argc[]) {
    monya::IndexVector<double> t = monya::IndexVector<double>();

    constexpr unsigned NELEM = 1000;
    std::vector<double> v;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    for (unsigned i = 0; i < NELEM; i++)
        v.push_back(distribution(generator));

    monya::IndexVector<double> c = monya::IndexVector<double>(v);
    for (unsigned i = 0; i < NELEM; i++) {
        assert(c[i].get_val() == v[i] && c[i].get_index() == i);
    }
    std::cout << "Test successful!\n";

    return EXIT_SUCCESS;
}
