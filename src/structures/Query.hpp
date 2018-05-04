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
#include "../BinaryTreeProgram.hpp"
#include <memory>

namespace monya { namespace container {

class BinaryNode;

// Inherit from this class to query
class Query {

};

class ProximityQuery: public Query {
    private:
        short k; // k nearest
        BinaryNode* qnode;
        std::vector<BinaryNode*> result;

        void append_response_node(BinaryNode* node) {
            result.push_back(node);
        }

        void append_response_node_lineage(BinaryNode* node);

    public:
        typedef std::shared_ptr<ProximityQuery> ptr;

        static ptr create() {
            return ptr(new ProximityQuery());
        }

        static ptr create(const short k) {
            return ptr(new ProximityQuery(k));
        }

        static ptr create(BinaryNode* nodeshell, const short k) {
            return ptr(new ProximityQuery(nodeshell, k));
        }

        ProximityQuery() {
        }

        ProximityQuery(const short k) {
            this->k = k;
        }

        ProximityQuery(BinaryNode* nodeshell, const short k) :
            ProximityQuery(k) {
                this->qnode = nodeshell;
            }

        void set_qnode(BinaryNode* qnode) {
            this->qnode = qnode;
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

        // TODO: Actually find what we're looking for
        template <typename TreeProgramType>
            void query(TreeProgramType tpt) {
                assert(NULL != this->qnode);
                BinaryNode* ret = tpt->find(this->qnode);
                result.push_back(ret);
            }
};
} }
#endif
