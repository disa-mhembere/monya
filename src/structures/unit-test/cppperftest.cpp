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

#include <map>
#include <algorithm>

#include <time.h>
#include <stdio.h>

#include "Tree.hpp"
#include "../common/io.hpp"

namespace mc = monya::container;

void insert_test(std::map<mc::NodeView<double>*, long>& tree, std::vector<long>& data) {
    clock_t t = clock();

    for (std::vector<long>::iterator it = data.begin();
            it != data.end(); ++it) {
        tree.insert(std::pair<mc::NodeView<double>*, long>(new mc::BinaryNode<double>(*it), *it));
        //tree.insert(std::pair<long, mc::NodeView<double>*>(*it, new mc::BinaryNode<double>(*it)));
    }

    t = clock() - t;
    printf ("It took %f sec to insert the %lu entries into a cpp map.\n",
            ((float)t)/CLOCKS_PER_SEC, data.size());
}

void query_test(std::map<long, mc::NodeView<double>*>& tree, std::vector<long> data) {
    std::cout << "Doing random shuffle ..... ";
    std::srand(1234);
    std::random_shuffle (data.begin(), data.end());
    std::cout << "Done!\n\n";
    clock_t t = clock();

    std::map<long, mc::NodeView<double>*>::iterator map_it;

    for (std::vector<long>::iterator it = data.begin();
            it != data.end(); ++it) {
        tree.find(*it);
    }

    t = clock() - t;
    printf ("\nIt took %f sec to lookup keys in std::map\n",((float)t)/CLOCKS_PER_SEC);

    // Delete them now
    for (std::map<long, mc::NodeView<double>*>::iterator it = tree.begin();
            it != tree.end(); ++it) {
        delete(it->second);
    }
}

int main(int argc, char* argv[]) {
#if 0
    std::vector<long> data(10);
    monya::bin_rm_reader<long> br("ordered_tree_10.bin");
#else
    std::vector<long> data(100000);
    monya::bin_rm_reader<long> br("ordered_tree_100k.bin");
#endif
    clock_t t = clock();
    br.read(data);
    t = clock() - t;
    printf ("It took %f sec to read the data.\n",((float)t)/CLOCKS_PER_SEC);
    t = clock();
    
    std::map<mc::NodeView<double>*, long> tree;

    // Test insert speed
    insert_test(tree, data);

    //// Test query speed
    //query_test(tree, data);

    return EXIT_SUCCESS;
}

