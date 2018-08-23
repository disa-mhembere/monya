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

#include "vecs_reader.hpp"

using namespace monya;

int main(int argc, char* argv[]) {
    constexpr size_t nrow = 10000;
    constexpr size_t ncol = 128;

    io::vecs_reader* vr = new io::fvecs_reader(
            "./siftsmall_base.fvecs", nrow, ncol);

    std::vector<data_t> v(nrow*ncol);
    vr->read(v);

    std::cout << "Echoing the first 2 rows ...\n";
    for (size_t row = 0; row < 5; row++) {
        std::cout << "[ ";
        for (size_t col = 0; col < ncol; col++) {
            std::cout << v[(row*ncol)+col] << " ";
        }
        std::cout << "]\n";
    }

    delete vr;
    std::cout << "Test vecs_reader w vector successful" << std::endl;

    vr = new io::fvecs_reader(
            "./siftsmall_base.fvecs", nrow, ncol);
    data_t* v2 = new data_t[nrow*ncol];
    vr->read(v2);

    for (size_t i = 0; i < nrow*ncol; i++)
        assert(v2[i] == v[i]);

    delete vr;
    delete [] v2;

    std::cout << "Test vecs_reader w array successful" << std::endl;

    return EXIT_SUCCESS;
}
