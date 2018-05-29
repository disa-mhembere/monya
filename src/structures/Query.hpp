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
#include "SampleVector.hpp"
#include <memory>

namespace monya {
    class BinaryTreeProgram;
    namespace container {

class BinaryNode;

// Inherit from this class to query
class Query {

    public:
        typedef std::shared_ptr<Query> ptr;
        virtual void print() = 0;
        virtual void run(BinaryTreeProgram* tpt) = 0;
};

class ProximityQuery: public Query {
    private:
        short k; // k nearest
        SampleVector::ptr qsample;
        std::vector<BinaryNode*> result;

        void append_response_node(BinaryNode* node) {
            result.push_back(node);
        }

        void append_response_node_lineage(BinaryNode* node);

    public:

        static ptr create() {
            return Query::ptr(new ProximityQuery());
        }

        static ptr create(const short k) {
            return ptr(new ProximityQuery(k));
        }

        static ptr create(SampleVector::ptr qs, const short k) {
            return ptr(new ProximityQuery(qs, k));
        }

        ProximityQuery() {
        }

        ProximityQuery(const short k) {
            this->k = k;
        }

        ProximityQuery(SampleVector::ptr qsample, const short k) :
            ProximityQuery(k) {
                this->qsample = qsample;
            }

        void set_qsample(SampleVector::ptr qs) {
            qsample = qs;
        }

        void set_k(const short k) {
            this->k = k;
        }

        const short get_k() const {
            return k;
        }

        const std::vector<BinaryNode*>& get_query_result() const {
            return result;
        }

        void print() override;

        // Actually find what we're looking for
        void run(BinaryTreeProgram* tp) override;
};

} }
#endif
