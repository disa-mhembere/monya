/*
 * Copyright 2017 neurodata (http://neurodata.io/)
 * Written by Disa Mhembere (disa@cs.jhu.edu)
 *
 * This file is part of Monya
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY CURRENT_KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MONYA_DISTANCE_HPP__
#define MONYA_DISTANCE_HPP__

#include <cstdlib>
#include "types.hpp"

namespace monya {

    class distance {
        public:
            static data_t euclidean(data_t* arr,
                data_t* other, const size_t nelem) {
            data_t res = 0;
            for (size_t i = 0; i < nelem; i++) {
                auto tmp = arr[i] - other[i];
                res += (tmp*tmp);
            }
            return sqrt(res);
        }
    };

} // End namespace monya
#endif // MONYA_DISTANCE_HPP__
