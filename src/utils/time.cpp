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

#include "time.hpp"
#include <iostream>

namespace monya { namespace utils {
    time::time(): flag(false) {}

    void time::tic() {
        start = std::chrono::high_resolution_clock::now();
        flag = true;
    }

    double time::len() {
        return duration.count();
    }

    double time::toc() {
        if (!flag)
            std::cerr << "[WARNING]: time(): multiple calls to `toc`\n";
        flag = false;
        duration = std::chrono::high_resolution_clock::now() - start;
        return duration.count();
    }
} } // End namespace
