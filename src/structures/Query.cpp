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

#include "Query.hpp"
#include "BinaryNode.hpp"
#include "../BinaryTreeProgram.hpp"

namespace monya { namespace container {

    void ProximityQuery::append_response_node_lineage(BinaryNode* node) {
        // TODO: Traverse the node and return

        if (NULL == node)
            return;
    }

    void ProximityQuery::print() {
        std::cout << "ProximityQuery:\n";
        std::cout << "k: " << k << "\n" <<
            "query: "; qsample->print(); std::cout << "\n";
    }

    // Actually find what we're looking for
    void ProximityQuery::run(BinaryTreeProgram* tp) {
        assert(!qsample->empty());
        tp->find_neighbors(this);

        //std::vector<BinaryNode> ret = tpt->(this->qnode);
        //result.push_back(ret);
    }
}} // End namespace monya::container
