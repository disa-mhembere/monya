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

#ifndef MONYA_UTILITY_NODE_HPP__
#define MONYA_UTILITY_NODE_HPP__

#include <limits>
#include <algorithm>
#include "../common/types.hpp"

namespace monya { namespace utils {

template<class T=data_t>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    approx_equal(T x, T y, T ulp =0.1) {
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y)
        * ulp || std::abs(x-y) < std::numeric_limits<T>::min();
}

int get_num_nodes();
} } // End namespace monya::utils
#endif
