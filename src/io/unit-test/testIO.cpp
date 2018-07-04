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

    std::vector<data_t> v;
    for(offset_t i = 0; i < NROW*NCOL; i++)
        v.push_back(i);

    std::cout << "Creating MemoryIO unit ...\n";
    io::IO::raw_ptr memioer = new io::MemoryIO(&v[0],
            dimpair(NROW, NCOL), MAT_ORIENT::ROW);

    std::string fn = std::string("inmem_n") + std::to_string(NROW) +
        std::string("_m") + std::to_string(NCOL) + std::string(".bin");
    memioer->set_fn(fn);

    std::cout << "\nROW wise print:\n";
    assert(memioer->get_orientation() == MAT_ORIENT::ROW);
    assert(memioer->shape().first == NROW
            && memioer->shape().second == NCOL);
    io::print_mat<data_t>(io::MemoryIO::cast2(memioer)->get_data(),
            memioer->shape().first, memioer->shape().second);

    memioer->transpose();
    std::cout << "\nCOL wise print:\n";
    assert(memioer->get_orientation() == MAT_ORIENT::COL);
    assert(memioer->shape().first == NCOL
            && memioer->shape().second == NROW);
    io::print_mat<data_t>(io::MemoryIO::cast2(memioer)->get_data(),
            memioer->shape().first, memioer->shape().second);

    memioer->transpose();
    std::cout << "\nBack to ROW wise print:\n";
    assert(memioer->shape().first == NROW
            && memioer->shape().second == NCOL);
    io::print_mat<data_t>(io::MemoryIO::cast2(memioer)->get_data(),
            memioer->shape().first, memioer->shape().second);

    std::cout << "Writing to file ...\n";
    memioer->write();

    assert(NROW == memioer->shape().first);
    assert(NCOL == memioer->shape().second);

    std::cout << "\nReading back through SyncIO unit...\n";
    std::vector<data_t> v2(NROW*NCOL);
    io::IO::raw_ptr syncioer = new io::SyncIO(fn, dimpair(NROW, NCOL),
            MAT_ORIENT::ROW);

    syncioer->read(&v2[0]);

    std::cout << "SyncIO echo: \n";
    io::print_mat<data_t>(&v2[0], syncioer->shape().first,
            syncioer->shape().second);

    data_t* membuf = NULL;
    data_t* syncbuf = NULL;

    ////////////////////////////////////////////////////////////////////////////
    // Test get_col
    ////////////////////////////////////////////////////////////////////////////
    for (size_t col = 0; col < NCOL; col++) {
        membuf = memioer->get_col(col);
        syncbuf = syncioer->get_col(col);

        for (size_t row = 0; row < NROW; row++) {
            assert(membuf[row] == v[row*NCOL+col]);
            assert(membuf[row] == v2[row*NCOL+col]);
            assert(membuf[row] == syncbuf[row]);
        }

        if (memioer->get_orientation() == MAT_ORIENT::ROW)
            delete [] membuf;
        delete [] syncbuf; // Always
    }

    ////////////////////////////////////////////////////////////////////////////
    // Test get_row
    ////////////////////////////////////////////////////////////////////////////

#if 1
    for (size_t row = 0; row < NROW; row++) {
        membuf = memioer->get_row(row);
        syncbuf = syncioer->get_row(row);

        for (size_t col = 0; col < NCOL; col++) {
            assert(membuf[col] == v[row*NCOL+col]);
            assert(membuf[col] == v2[row*NCOL+col]);
            assert(membuf[col] == syncbuf[col]);
        }
        delete [] syncbuf; // Always
    }
#endif

    std::cout << "Cleaning up\n";
    syncioer->destroy();
    assert(!std::remove(fn.c_str()));
    memioer->destroy();

    std::cout << "Test successful" << std::endl;
    return EXIT_SUCCESS;
}
