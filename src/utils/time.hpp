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

#ifndef MONYA_TIME_HPP__
#define MONYA_TIME_HPP__

#include <chrono>
#include <ctime>

namespace monya { namespace utils {
class time {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
        bool flag;
        std::chrono::duration<double> duration;

    public:
        time(): flag(false) {}

        void tic() {
            start = std::chrono::high_resolution_clock::now();
            flag = true;
        }

        double len() {
            return duration.count();
        }

        double toc() {
            if (!flag)
                std::cerr << "[WARNING]: time(): multiple calls to `toc`\n";
            flag = false;
            duration = std::chrono::high_resolution_clock::now() - start;
            return duration.count();
        }
};

} } // End namespace
#endif
