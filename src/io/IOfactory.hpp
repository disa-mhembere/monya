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
#include "../common/types.hpp"

namespace monya {
class IOfactory {
    private:
        std::string fn; // Either name in safs or OS file system
        bool is_direct;

    public:
        static io::IO::raw_ptr create(IOTYPE iotype) {

            switch (iotype) {
                case MEM:
                    return new io::MemoryIO();
                    break;
                case SEM:
                    throw not_implemented_exception();
                    break;
                case SYNC:
                    return new io::SyncIO();
                    break;
                default:
                    throw not_implemented_exception();
            }
        }
};
}

#endif
