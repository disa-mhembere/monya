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
#ifndef MONYA_METRIC_HPP__
#define MONYA_METRIC_HPP__

#include <vector>
#include <cassert>
#include <numeric>

#include "types.hpp"
#include "NNVector.hpp"

namespace monya {

// Determine how approximate our NN are compared to exact
class Metric {
    public:
        // Given a single kNN query we get a vector of length
        //  k with NN. This test the difference. Order shouldn't matter
        static data_t get_raw_accuracy(IndexVector& approx,
                IndexVector& exact) {

            // Order doesn't matter
            assert(approx.size() == exact.size());
            std::vector<bool> matches(approx.size()); // bitmap
            matches.assign(approx.size(), false);

            for (size_t i = 0; i < approx.size(); i++) {
                if (approx.find(exact[i])) {
                    matches[i] = true;
                }
            }

            size_t match_count = std::accumulate(matches.begin(),
                    matches.end(), 0);
            if (match_count)
                return match_count/((data_t)matches.size());
            return 0;
        }

        // Given a single kNN query we get a vector of length
        //  k with NN. This test the difference. Order shouldn't matter
        static data_t get_raw_accuracy(NNVector& approx,
                NNVector& exact) {

            // Order doesn't matter
            assert(approx.size() == exact.size());
            std::vector<bool> matches(approx.size()); // bitmap
            matches.assign(approx.size(), false);

            for (size_t i = 0; i < approx.size(); i++) {
                if (approx.find(exact[i])) {
                    matches[i] = true;
                }
            }

            size_t match_count = std::accumulate(matches.begin(),
                    matches.end(), 0);
            if (match_count)
                return match_count/((data_t)matches.size());
            return 0;
        }
};
}

#endif
