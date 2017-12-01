#!/usr/bin/env python

# Copyright 2017 neurodata (http://neurodata.io/)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from genband import GenBand
from BandMatrix import BandMatrix
import numpy as np
from time import time
from scipy import sparse
import warnings
warnings.filterwarnings('ignore')

np.set_printoptions(3, linewidth=100)

def test(n, l, u):
    generator = GenBand(n, l, u)
    band_mat = generator.generate()
    # x = np.random.random((n,1))
    x = sparse.rand(n, 1, .5).todense()

    dense_mat = generator.raw()
    start = time()
    dx = np.dot(dense_mat, x)
    print "{},".format(time()-start),

    start = time()
    bx = band_mat*x
    print "{}".format(time()-start)

    # print "dense_mat:\n", dense_mat
    # print "band_mat:\n", band_mat
    # print "bx\n", bx
    # print "dx\n", dx

    # assert np.all(np.isclose(bx.flatten(), dx.flatten()))

if __name__ == "__main__":
    n = 10000
    step = 100
    stop = 5000
    print "Bandwith, Dense Mult, Band Mult"
    for bandwidth in xrange(step, n, step):
        print "{},".format(bandwidth),
        test(n, bandwidth/2, bandwidth/2)
