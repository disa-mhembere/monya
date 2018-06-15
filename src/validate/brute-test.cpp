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

#include "BruteForcekNN.hpp"
#include "../io/IO.hpp"

using namespace monya;
using namespace monya::test;

int main (int argc, char** argv) {

    size_t nsamples = 32;
    size_t nfeatures = 16;
    std::string fn = "/Research/monya/src/test-data/rand_32_16.bin";
    std::vector<data_t> data(nsamples*nfeatures);

    io::IO* ioer = new io::SyncIO(fn, dimpair(32, 16), COL);
    static_cast<io::SyncIO*>(ioer)->read(&data[0]);

    std::cout << "Test print of data:\n";
    io::print_mat<data_t>(&data[0], nsamples, nfeatures);
    std::cout << "\n";

    BruteForcekNN bf (&data[0], nsamples, nfeatures);

    constexpr size_t k = 5;
    // Test kNN
    for (sample_id_t sid = 0; sid < nsamples; sid++) {
        auto iv = bf.getNN(&data[sid*nfeatures], k);
        std::cout << k <<"-NN of sid: " << sid;
        io::print_arr<data_t>(&data[sid*nfeatures], nfeatures);
        iv.print();
    }

    ioer->destroy();
    return 0;
}
