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

#ifndef MONYA_VECS_READER_HPP__
#define MONYA_VECS_READER_HPP__

#include <fstream>

#include "../common/exception.hpp"
#include "../common/types.hpp"

namespace monya { namespace io {

    enum vecs_t {
        FVECS,
        IVECS,
        BVECS
    };

    class vecs_reader {
        protected:
            std::fstream fs;
            dimpair dim;

        public:
            vecs_reader(const std::string fn, const size_t nrow,
                    const size_t ncol) {
                dim = dimpair(nrow, ncol);

                fs.open(fn.c_str(), std::ios_base::in);
                if (!fs.is_open())
                    throw io_exception(std::string("Failure to open file: '")
                            + fn + std::string("'\n"));
            }

            virtual void read(std::vector<data_t>& buf) = 0;
            virtual void read(data_t* buf) = 0;
            virtual ~vecs_reader() { fs.close();}
    };

    class fvecs_reader : public vecs_reader {
        public:
            fvecs_reader(const std::string fn, const size_t nrow,
                    const size_t ncol) : vecs_reader(fn, nrow, ncol) {
            }

            void read(std::vector<data_t>& buf) override {
                read(&buf[0]);
            }

            void read(data_t* buf) override {
                int read_dim;
                for (size_t row = 0; row < dim.first; row++) {
                    fs.read(reinterpret_cast<char*>(&read_dim), sizeof(buf[0]));
                    assert(read_dim == static_cast<int>(dim.second));

                    fs.read(reinterpret_cast<char*>((&buf[row*dim.second])),
                            dim.second*sizeof(buf[0]));
                }
            }
    };

} } // End namespace monya::io
#endif
