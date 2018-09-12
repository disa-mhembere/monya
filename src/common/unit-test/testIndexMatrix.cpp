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

    constexpr unsigned NELEM = 10;
    std::vector<data_t> v, v2;

    std::default_random_engine generator;
    std::uniform_real_distribution<data_t> distribution(0.0, 1.0);

    // Test setting values
    for (unsigned i = 0; i < NELEM; i++) {
        v.push_back(distribution(generator));
        v2.push_back(v.back() + 1.0);
    }

    IndexMatrix c;
    c.set_indexes(&v[0], 4, v.size());
    c.set_indexes(&v2[0], 55, v2.size());

    for (unsigned i = 0; i < NELEM; i++) {
        assert(c[4][i].get_val() == v[i] && c[4][i].get_index() == i);
        assert(c[55][i].get_val() == v2[i] && c[55][i].get_index() == i);
    }

    // Test setting indexes
    std::vector<sample_id_t> v3;
    for (sample_id_t i = 0; i < NELEM; i++)
        v3.push_back(i);

    assert(v3.size() == v2.size());
    std::random_shuffle (v3.begin(), v3.end());
    c.set_indexes(&v3[0], 132, v3.size());
    // Set the values for these indexes
    for (size_t i = 0; i < v2.size(); i++)
        c[132][i].set_val(v2[i]);

    c.print();

    for (size_t i = 0; i < v2.size(); i++) {
        assert(c[132][i].get_val() == v2[i] && c[132][i].get_index() == v3[i]);
    }

    std::cout << "TestIndexMatrix successful!\n";
    return EXIT_SUCCESS;
}
