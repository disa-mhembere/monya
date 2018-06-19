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

#include <vector>
#include <cassert>
#include <cmath>

#include "../common/monya.hpp"
#include "../common/types.hpp"

namespace monya { namespace test {
// The idea is to collect all the data and pass it this class
//  then compute all pairwise distances. This is not fast or
//  scalable!
class BruteForcekNN {

    private:
        data_t* data;
        size_t nsamples;
        size_t nfeatures;

    public:
        BruteForcekNN(data_t* _data, const sample_id_t _nsamples,
                const size_t _nfeatures):
            data(_data), nsamples(_nsamples), nfeatures(_nfeatures) {
        }

        IndexVector getNN(data_t* sample, const long k) {
            IndexVector NN;

            for(size_t i = 0; i < nsamples; i++) {
                auto dist = euclidean_distance(sample, &data[i*nfeatures],
                        nfeatures);

                if (i < (unsigned)k) {
                    NN.append(i, dist);
                } else {
                    if (i == (unsigned) k)
                        NN.sort();

                    long repl_indx = -1;
                    for (long j = k - 1; j >= 0; j--) {
                        if (dist < NN[j].get_val()) {
                            repl_indx = j;
                        }
                    }

                    if (repl_indx >= 0) {
                        NN.insert(IndexVal<data_t>(i, dist), repl_indx);
                    }
                }
            }

            if (NN.size() > (unsigned)k)  {
                NN.trim(k);
            }
            return NN;
        }

        data_t euclidean_distance(data_t* arr, data_t* other,
                const size_t nelem) {
            data_t res = 0;
            for (size_t i = 0; i < nelem; i++) {
                auto tmp = arr[i] - other[i];
                res += (tmp*tmp);
            }
            return sqrt(res);
        }
};
}}
