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

#ifndef __MONYA_THREAD_STATE_HPP__
#define __MONYA_THREAD_STATE_HPP__

#include <string>
#include "../common/exception.hpp"

namespace monya {
    enum ThreadState_t {
        TEST, /* Just for testing*/
        BUILD, /* Tree building moving data for reduces rma*/
        QUERY, /* Querying the tree */
        WAIT, /* When the thread is waiting for a new task*/
        EXIT /* Say goodnight */
    };

    class str {
        public:
            static std::string to(const enum ThreadState_t state) {
                switch(state) {
                    case TEST:
                        return "TEST";
                    case BUILD:
                        return "BUILD";
                    case QUERY:
                        return "QUERY";
                    case WAIT:
                        return "WAIT";
                    case EXIT:
                        return "EXIT";
                    default:
                        throw parameter_exception(
                                "Unknown state", __FILE__, __LINE__);
                }
            }
    };
}

#endif
