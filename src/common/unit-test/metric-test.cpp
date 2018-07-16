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

#include "../Metric.hpp"

using namespace monya;

void test_metric() {
    {
    // Test 1 correct of 3
    IndexVector exact;
    exact.append(0, .1);
    exact.append(2, .3);
    exact.append(4, .6);

    std::cout << "Printing exact:\n";
    exact.print();

    IndexVector approx;
    approx.append(0, .1);
    approx.append(2, .2);
    approx.append(4, .6);

    std::cout << "Printing approx:\n";
    approx.print();

    auto ra = Metric::get_raw_accuracy(approx, exact);
    std::cout << "Raw accuracy: " << ra << std::endl;
    assert(ra == (double)2/3);
    }

    {
    // All correct
    IndexVector exact;
    exact.append(0, .1);
    exact.append(22, 1.3);
    exact.append(4, 99.6);
    exact.append(25, 22.6);
    exact.append(104, 12.6);

    std::cout << "\nPrinting exact:\n";
    exact.print();

    IndexVector approx;
    approx.append(4, 99.6);
    approx.append(0, .1);
    approx.append(25, 22.6);
    approx.append(22, 1.3);
    approx.append(104, 12.6);

    std::cout << "Printing approx:\n";
    approx.print();

    auto ra = Metric::get_raw_accuracy(approx, exact);
    std::cout << "Raw accuracy: " << ra << std::endl;
    assert(ra == (double)1);
    }

    {
    // Two incorrect
    IndexVector exact;
    exact.append(0, .1);
    exact.append(22, 1.3);
    exact.append(4, 99.6);
    exact.append(25, 22.6);
    exact.append(104, 12.6);

    std::cout << "\nPrinting exact:\n";
    exact.print();

    IndexVector approx;
    approx.append(5, 199.6);
    approx.append(0, .1);
    approx.append(25, 22.6);
    approx.append(221, 11.3);
    approx.append(104, 12.6);

    std::cout << "Printing approx:\n";
    approx.print();

    auto ra = Metric::get_raw_accuracy(approx, exact);
    std::cout << "Raw accuracy: " << ra << std::endl;
    assert(ra == 3/(double)5);
    }
}

int main(int argc, char** argv) {
    test_metric();
    return EXIT_SUCCESS;
}
