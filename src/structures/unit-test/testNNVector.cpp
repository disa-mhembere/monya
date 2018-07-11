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

#include "NNVector.hpp"
#include "../../common/types.hpp"

// Being lazy
using namespace monya;
using namespace monya::container;

int main() {

#if 0
    // Empty
    {
        NNVector nnv(0);
        assert(!nnv.size());
    }

    // One
    {
        NNVector nnv(1);
        constexpr size_t ind = 10;
        constexpr data_t val = 22.3;

        IndexVal<data_t> iv(ind, val);
        nnv.eval(iv);
        nnv.print();
        assert(nnv.size() == 1);
        assert(nnv[0].get_index() == ind && nnv[0].get_val() == val);

        // Shouldn't replace NN
        IndexVal<data_t> iv2(0, 22.31);
        nnv.eval(iv2);
        assert(nnv.size() == 1);
        assert(nnv[0].get_index() == ind && nnv[0].get_val() == val);

        // SHOULD replace NN
        IndexVal<data_t> iv3(1, 21);
        nnv.eval(iv3);
        assert(nnv.size() == 1);
        nnv.print();
        assert(nnv[0].get_index() == 1 && nnv[0].get_val() == 21);
    }
#endif

    // Test 5 without replacement (check sorting)
    {
        NNVector nnv(5);
        IndexVal<data_t> iv0(6, 2);
        nnv.eval(iv0);
        assert(nnv.size() == 1);

        IndexVal<data_t> iv1(5, 1);
        nnv.eval(iv1);
        assert(nnv.size() == 2);
        assert(nnv[0].get_index() == 5 && nnv[0].get_val() == 1);

        IndexVal<data_t> iv2(8, 4.0);
        nnv.eval(iv2);
        assert(nnv.size() == 3);
        assert(nnv[2].get_index() == 8 && nnv[2].get_val() == 4.0);

        IndexVal<data_t> iv3(7, 3);
        nnv.eval(iv3);
        assert(nnv.size() == 4);
        assert(nnv[2].get_index() == 7 && nnv[2].get_val() == 3);
        nnv.print();

        IndexVal<data_t> iv4(4, 0);
        nnv.eval(iv4);
        nnv.print();

        for (sample_id_t i = 0; i < 5; i++)
            assert((nnv[i].get_index() == (i+4)) &&
                    (nnv[i].get_val() == (data_t)i));
    }

    printf("NNVector test successful!\n");
    return EXIT_SUCCESS;
}
