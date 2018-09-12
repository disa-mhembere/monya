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
#include <cstdlib>
#include "IOfactory.hpp"
#include <cassert>
#include <vector>

using namespace monya;

int main(int argc, char* argv[]) {
    // Test Sync
    io::IO::raw_ptr ioer = IOfactory::create(io_t::SYNC);
    assert(ioer->get_fn().empty());
    ioer->destroy();

    constexpr size_t NROW = 8;
    constexpr size_t NCOL = 16;
    std::vector<data_t> d;

    for (size_t i = 0; i < NROW*NCOL; i++) {
        d.push_back(i);
    }

    // Test memory
    ioer = IOfactory::create(io_t::MEM);
    ioer->shape(dimpair(NROW, NCOL));
    io::MemoryIO::cast2(ioer)->set_data(&d[0]);
    ioer->set_orientation(orient_t::COL);

    data_t *buf = NULL;

    for (size_t col = 0; col < NCOL; col++) {
        buf = static_cast<data_t*>(ioer->get_col(col));

        for (size_t row = 0; row < NROW; row++)
            assert(buf[row] == d[NROW*col + row]);
    }

    ioer->destroy();
    std::cout << "Test complete\n";
    return EXIT_SUCCESS;
}
