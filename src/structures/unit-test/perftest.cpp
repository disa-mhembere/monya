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

#include "Tree.hpp"
#include "BinaryNode.hpp"
#include "../common/io.hpp"
#include <time.h>
#include <stdio.h>

#ifdef PROFILER
#include <gperftools/profiler.h>
#endif

namespace mc = monya::container;


void insert_test(mc::Tree<double>::ptr tree, std::vector<long>& data) {
    clock_t t = clock();

    if (data.size() < 1)
        return;

    tree->set_root(new mc::BinaryNode<double>(data[0]));

    mc::BinaryNode<double>* root = mc::BinaryNode<double>::cast2(tree->get_root());
    for (std::vector<long>::iterator it = data.begin() + 1;
            it != data.end(); ++it) {

        mc::BinaryNode<double>* node = new mc::BinaryNode<double>(*it);
        tree->insert(node, root);
        //std::cout << *it << " ";
    }
    t = clock() - t;
    printf ("\nIt took %f sec to insert the data into a Monya Tree.\n",
            ((float)t)/CLOCKS_PER_SEC);
}

void query_test(mc::Tree<double>::ptr tree, std::vector<long> data) {
    std::cout << "Doing random shuffle ..... ";
    std::srand(1234);
    std::random_shuffle (data.begin(), data.end());
    std::cout << "Done!\n";
    clock_t t = clock();

    for (std::vector<long>::iterator it = data.begin();
            it != data.end(); ++it) {
        tree->find(*it); 
    }

    t = clock() - t;
    printf ("\nIt took %f sec to lookup keys in Monya\n",((float)t)/CLOCKS_PER_SEC);
}

int main(int argc, char* argv[]) {

#ifdef PROFILER
    ProfilerStart("perftest.perf");
#endif

#if 0
    std::vector<long> data(10);
    monya::bin_rm_reader<long> br("ordered_tree_10.bin");
#else
    std::cout << "Reading 100k dataset\n";
    std::vector<long> data(100000);
    monya::bin_rm_reader<long> br("ordered_tree_100k.bin");
#endif

    clock_t t = clock();
    br.read(data);
    t = clock() - t;
    printf ("It took %f sec to read the data.\n",((float)t)/CLOCKS_PER_SEC);

    mc::Tree<double>::ptr tree = mc::Tree<double>::create();
    insert_test(tree, data);

    // Make sure we inserted correctly
    //printf("Echoing tree\n");
    //tree->echo();

    // Now query that we get back the same thing
    query_test(tree, data);


#ifdef PROFILER
   ProfilerStop(); 
#endif
   return EXIT_SUCCESS;
}

