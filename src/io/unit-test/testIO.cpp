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

#include "IO.hpp"
#include <vector>
#include <cassert>
#include <cstdio>

using namespace monya;

int main(int argc, char* argv[]) {

    constexpr offset_t NROW = 8;
    constexpr offset_t NCOL = 4;

    std::vector<offset_t> v;
    for(offset_t i = 0; i < NROW*NCOL; i++)
        v.push_back(i);

    std::cout << "Creating MemoryIO unit ...\n";
    io::IO::raw_ptr memioer = new io::MemoryIO(&v[0],
            dimpair(NROW, NCOL), MAT_ORIENT::COL,
            sizeof(offset_t));

    std::string fn = std::string("inmem_n") + std::to_string(NROW) +
        std::string("_m") + std::to_string(NCOL) + std::string(".bin");
    memioer->set_fn(fn);

    std::cout << "Writing to file ...\n";
    memioer->write();

    assert(NROW == memioer->shape().first);
    assert(NCOL == memioer->shape().second);

    std::cout << "Reading back through SyncIO unit...\n";
    std::vector<offset_t> v2(NROW*NCOL);
    io::IO::raw_ptr syncioer = new io::SyncIO(fn, dimpair(NROW, NCOL),
            MAT_ORIENT::COL, sizeof(offset_t));

    syncioer->read(&v2[0]);

    offset_t* buf = NULL;
    offset_t* buf2 = NULL;

    for (size_t col = 0; col < NCOL; col++) {
        buf = static_cast<offset_t*>(memioer->get_col(col));
        buf2 = static_cast<offset_t*>(syncioer->get_col(col));

        for (size_t row = 0; row < NROW; row++) {
            assert(buf[row] == v[NROW*col + row]);
            assert(buf[row] == v2[NROW*col + row]);
            assert(buf2[row] == v[NROW*col + row]);
        }
    }

    std::cout << "Cleaning up\n";
    memioer->destroy();
    syncioer->destroy();
    assert(!std::remove(fn.c_str()));

    std::cout << "Test successful" << std::endl;
    return EXIT_SUCCESS;
}
