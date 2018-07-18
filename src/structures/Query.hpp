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

#ifndef MONYA_QUERY_HPP__
#define MONYA_QUERY_HPP__

#include "../common/types.hpp"
#include <memory>

namespace monya {
    class BinaryTreeProgram;
    class NNVector;
    namespace container {

class BinaryNode;
class SampleVector;

// Inherit from this class to query
class Query {

    public:
        virtual void print() = 0;
        virtual void run(BinaryTreeProgram* tpt) = 0;
        virtual data_t& operator[](const size_t idx) = 0;
        virtual ~Query() { }
};

class ProximityQuery: public Query {
    private:
        //const short k; // k nearest
        short k; // k nearest
        SampleVector* qsample;
        NNVector* result;

    public:

        ProximityQuery(const short k);
        ProximityQuery(SampleVector* qsample, const short k);
        data_t& operator[](const size_t idx) override;
        void eval(const size_t id, const data_t dist);

        static ProximityQuery* raw_cast(Query* q) {
            return static_cast<ProximityQuery*>(q);
        }

        void set_qsample(SampleVector* qs) {
            qsample = qs;
        }

        SampleVector* get_qsample() {
            return qsample;
        }

        const short get_k() const {
            return k;
        }

        NNVector* getNN() const {
            return result;
        }

        void print() override;

        // Actually find what we're looking for
        void run(BinaryTreeProgram* tp) override;

        ~ProximityQuery() override;
};
} } // End namespace monya::container
#endif
