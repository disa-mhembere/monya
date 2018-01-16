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

#ifndef MONYA_IO_FACTORY_HPP
#define MONYA_IO_FACTORY_HPP

#include "IO.hpp"

namespace monya {
class iofactory {
    private:
        std::string fn; // Either name in safs or OS file system
        bool is_direct;

    public:
        template <typename T>
        typename io::IO<T>::ptr create_synchronous() {
            is_direct = true;
            return io::SyncIO<T>::create();
        }

        //typename io::IO<T>::ptr create_asynchronous(bool isdirect) {
            // TODO
        //}
};
}

#endif
