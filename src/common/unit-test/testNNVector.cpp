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

#include <random>
#include <algorithm>
#include "../NNVector.hpp"

// Being lazy
using namespace monya;

int main() {

#if 1
    // Empty
    {
        NNVector nnv(0);
        assert(!nnv.size());
    }
#endif

#if 1
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

#if 1
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
#endif

    // Test with replacement (general case)
    {
        constexpr size_t k = 50;
        constexpr size_t choose = 200;
        std::vector<data_t> elems;

        NNVector nnv(k);

        std::default_random_engine generator;
        std::uniform_real_distribution<data_t> distribution(0.0, 100.0);

        for (size_t i = 0; i < choose; i++) {
            elems.push_back(distribution(generator));
            IndexVal<data_t> iv(i, elems.back());
            nnv.eval(iv);
        }

        printf("\n\nGeneral Test\n\n");
        nnv.print();
        std::sort(elems.begin(), elems.end());

        for (size_t i = 0; i < k; i++) {
            std::cout << "nnv: " << nnv[i].get_val() << ", elems: " <<
                elems[i] << std::endl;

            assert(nnv[i].get_val() == elems[i]);
        }

        //  Test cache
        std::vector<size_t> acc_patt{1, 2, 3, 0, 4, 5, 9, 9, 8, 7, 8, 2, 3};
        for (size_t i : acc_patt) {
            std::cout << "Index: " << i << ", nnv: " << nnv[i].get_val() <<
                ", elems: " << elems[i] << "\n";
            assert(nnv[i].get_val() == elems[i]);
        }

        // Random cache test
        std::default_random_engine gen2;
        std::uniform_int_distribution<int> dist2(0, k-1);
        for (int i = 0; i < (int)choose; i++) {
            int index = dist2(gen2);
            assert(nnv[index].get_val() == elems[index]);
        }
    }

    printf("NNVector test successful!\n");
    return EXIT_SUCCESS;
}
