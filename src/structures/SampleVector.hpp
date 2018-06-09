/*
 * copyright 2017 neurodata (https://neurodata.io)
 * written by disa mhembere (disa@cs.jhu.edu)
 *
 * this file is part of monya.
 *
 * licensed under the apache license, version 2.0 (the "license");
 * you may not use this file except in compliance with the license.
 * you may obtain a copy of the license at
 *
 *     http://www.apache.org/licenses/license-2.0
 *
 * unless required by applicable law or agreed to in writing, software
 * distributed under the license is distributed on an "as is" basis,
 * without warranties or conditions of any kind, either express or implied.
 * see the license for the specific language governing permissions and
 * limitations under the license.
 */

#ifndef MONYA_SAMPLE_VECTOR_HPP__
#define MONYA_SAMPLE_VECTOR_HPP__

#include <vector>
#include "../common/types.hpp"
#include "../io/IO.hpp"

namespace monya { namespace container {

    // An abstraction over a vector OR sparse vector
    class SampleVector {
        public:
            typedef SampleVector* raw_ptr;

            SampleVector() { }

            virtual data_t& operator[](size_t) = 0;
            virtual size_t size() = 0;
            virtual size_t nnz() = 0;
            virtual bool empty() = 0;
            virtual void populate(data_t*, const size_t) { }
            virtual void print() = 0;
    };

    class DenseVector : public SampleVector {
        private:
            std::vector<data_t> data;

        public:

            static raw_ptr create_raw() {
                return new DenseVector;
            }

            static raw_ptr create_raw(data_t* data, size_t nelem) {
                return new DenseVector(data, nelem);
            }

            static raw_ptr cast2(raw_ptr o) {
                return static_cast<DenseVector*>(o);
            }

            DenseVector() {
            }

            DenseVector(data_t* data, size_t nelem) {
                populate(data, nelem);
            }

            data_t& operator[](size_t idx) {
                return data[idx];
            }

            size_t size() {
                return data.size();
            }

            bool empty() {
                return data.empty();
            }

            size_t nnz() {
                return data.size();
            }

            void populate(data_t* _data, const size_t nelem) {
                data.resize(nelem);
                std::copy(_data, _data+nelem, data.begin());
            }

            void print() override {
                monya::io::print_arr(&data[0], data.size());
            }
    };
} }

#endif
