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
#include "SampleVector.hpp"
#include "../common/NNVector.hpp"

namespace monya { namespace container {

    ProximityQuery::ProximityQuery(const short k, const tree_t ntree)
        : k(k), ntree(ntree) {
            for (size_t i = 0; i < ntree; i++)
                result.push_back(new NNVector(k));
    }

    ProximityQuery::ProximityQuery(SampleVector* qsample, const short k,
            const tree_t ntree): ProximityQuery(k, ntree) {

        this->qsample = qsample;
    }

    void ProximityQuery::eval(const size_t id, const data_t dist,
            const tree_t tree_id) {
        result[tree_id]->eval(id, dist);
    }

    void ProximityQuery::print() {
        std::cout << "ProximityQuery:\n";
        std::cout << "k: " << k << "\n" <<
            "query: "; qsample->print(); std::cout << "\n";
    }

    // Actually find what we're looking for!
    // Called by Compute Engine!
    void ProximityQuery::run(BinaryTreeProgram* tp) {
        assert(!qsample->empty());
        tp->find_neighbors(this);
    }

    data_t& ProximityQuery::operator[](const size_t idx) {
        return (*qsample)[idx];
    }

    ProximityQuery::~ProximityQuery() {
        for (size_t i = 0; i < result.size(); i++)
            delete(result[i]);
    }
}} // End namespace monya::container
