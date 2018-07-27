/* * Copyright 2017 Neurodata (https://neurodata.io)
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

#include "RBTree.hpp"
#include "RBNode.hpp"
#include "../io/IO.hpp"
#include <time.h>
#include <stdio.h>

#ifdef PROFILER
#include <gperftools/profiler.h>
#endif

using namespace monya;
namespace mc = monya::container;
namespace mi = monya::io;

void insert_test(mc::RBTree::ptr tree,
        std::vector<data_t>& data) {
    clock_t t = clock();

    if (data.size() < 1)
        return;

    for (std::vector<data_t>::iterator it = data.begin(); it != data.end(); ++it) {
        tree->insert(new mc::RBNode(*it));
    }

    t = clock() - t;
    printf("\nIt took %f sec to insert the data into a Monya RBTree.\n",
            ((float)t)/CLOCKS_PER_SEC);
}

void query_test(mc::RBTree::ptr tree,
        std::vector<data_t> data) {
    std::cout << "Doing random shuffle ..... ";
    std::srand(1234);
    std::random_shuffle (data.begin(), data.end());
    std::cout << "Done!\n";
    clock_t t = clock();

    for (std::vector<data_t>::iterator it = data.begin();
            it != data.end(); ++it) {
        mc::RBNode* node = new mc::RBNode(*it);
        tree->find(node);
        delete node;
    }

    t = clock() - t;
    printf ("\nIt took %f sec to lookup keys in Monya\n",((float)t)/CLOCKS_PER_SEC);
}

int main(int argc, char* argv[]) {

#ifdef PROFILER
    ProfilerStart("perftest.perf");
#endif

    std::string fn = "";
    size_t DATALEN = 0;

#if 0
    fn = "data/ordered_tree_10.bin";
    DATALEN = 10;
#endif
#if 0
    fn = "data/ordered_tree_10K.bin";
    DATALEN = 10000;
#endif
#if 0
    fn = "data/ordered_tree_100K.bin";
    DATALEN = 100000;
#endif
#if 1
    fn =  "data/ordered_tree_1M.bin";
    DATALEN = 10000000;
#endif

    std::cout << "Reading " << DATALEN << " dataset in '" << fn << "' ...\n";
    std::vector<data_t> data(DATALEN);
    mi::SyncIO br(fn);

    clock_t t = clock();
    br.read(&data[0]);
    t = clock() - t;
    printf ("It took %f sec to read the data.\n",((float)t)/CLOCKS_PER_SEC);

    mc::RBTree::ptr tree =
        mc::RBTree::create();
    insert_test(tree, data);

    // Make sure we inserted correctly

    // Now query that we get back the same thing
    query_test(tree, data);


#ifdef PROFILER
   ProfilerStop();
#endif
   return EXIT_SUCCESS;
}
