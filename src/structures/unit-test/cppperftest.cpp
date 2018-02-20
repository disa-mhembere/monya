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

#include "../NodeView.hpp"
#include "../io/IO.hpp"

namespace mc = monya::container;
namespace mi = monya::io;

template <typename T>
class ConcreteNode : public mc::NodeView <T> {

    protected:
    void prep() override {}
    void run() override {}

    public:
    using mc::NodeView<T>::NodeView;
    void spawn() override {}
};

void insert_test(std::map<ConcreteNode<double>*, long>& tree,
        std::vector<long>& data) {
    clock_t t = clock();

    for (std::vector<long>::iterator it = data.begin();
            it != data.end(); ++it) {
        tree.insert(std::pair<ConcreteNode<double>*, long>(
                    new ConcreteNode<double>(*it), *it));
    }

    t = clock() - t;
    printf ("It took %f sec to insert the %lu entries into a cpp map.\n",
            ((float)t)/CLOCKS_PER_SEC, data.size());
}

void query_test(std::map<ConcreteNode<double>*, long>& tree,
        std::vector<long> data) {
    std::cout << "Doing random shuffle ..... ";
    std::srand(1234);
    std::random_shuffle (data.begin(), data.end());
    std::cout << "Done!\n\n";
    clock_t t = clock();

    for (std::vector<long>::iterator it = data.begin();
            it != data.end(); ++it) {
        ConcreteNode<double>* node = new ConcreteNode<double>(*it);
        tree.find(node);
        delete(node);
    }

    t = clock() - t;
    printf("\nIt took %f sec to lookup keys in std::map\n",
            ((float)t)/CLOCKS_PER_SEC);

    // Delete them now
    for (std::map<ConcreteNode<double>*, long>::iterator it = tree.begin();
            it != tree.end(); ++it) {
        delete(it->first);
    }
}

int main(int argc, char* argv[]) {
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
    fn =  "data/ordered_tree_10M.bin";
    DATALEN = 10000000;
#endif
#if 0
    fn = "data/data_9959.bin";
    DATALEN = 9959;
#endif
#if 0
    fn =  "data/ordered_tree_1M.bin";
    DATALEN = 10000000;
#endif

    std::cout << "Reading " << DATALEN << " dataset in '" << fn << "' ...\n";
    std::vector<long> data(DATALEN);
    mi::SyncIO br(fn, sizeof(long));

    clock_t t = clock();
    br.read(&data[0]);
    t = clock() - t;
    printf ("It took %f sec to read the data.\n",((float)t)/CLOCKS_PER_SEC);
    t = clock();

    std::map<ConcreteNode<double>*, long> tree;

    // Test insert speed
    insert_test(tree, data);

    // Test query speed
    query_test(tree, data);

    return EXIT_SUCCESS;
}
