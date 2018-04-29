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

#include "../monya.hpp"
#include <random>

#include <cassert>

using namespace monya;

int main(int argv, char* argc[]) {
    IndexVector t = IndexVector();

    constexpr unsigned NELEM = 1000000;
    std::vector<data_t> v;

    std::default_random_engine generator;
    std::uniform_real_distribution<data_t> distribution(0.0, 1.0);

    for (unsigned i = 0; i < NELEM; i++)
        v.push_back(distribution(generator));

    IndexVector c = IndexVector(v);
    for (unsigned i = 0; i < NELEM; i++)
        assert(c[i].get_val() == v[i] && c[i].get_index() == i);

    // Serial Sort test
    std::sort(v.begin(), v.end());
    std::sort(c.begin(), c.end());

    for (unsigned i = 0; i < NELEM; i++)
        assert(c[i].get_val() == v[i]);
    std::cout << "Serial sort test successful\n";

    // Shuffle
    std::random_shuffle (v.begin(), v.end());
    std::random_shuffle (c.begin(), c.end());
    // Parallel sort
    __gnu_parallel::sort(v.begin(), v.end());
    __gnu_parallel::sort(c.begin(), c.end());

    for (unsigned i = 0; i < NELEM; i++)
        assert(c[i].get_val() == v[i]);
    std::cout << "Parallel sort test successful\n";

    // Echo
    //for (auto i = c.begin(); i != c.end(); ++i)
        //std::cout << *i;

    std::cout << "Test successful!\n";
    return EXIT_SUCCESS;
}
