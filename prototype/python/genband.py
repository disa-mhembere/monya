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

import numpy as np
from scipy import sparse
from BandMatrix import BandMatrix

# NOTE: Sparse matrices only!
class GenBand():
    def __init__(self, n, l, u):
        """
        @param n: the number of rows/cols in the original square matrix
        @param l: the number of subdiagonals
        @param u: the number of superdiagonals
        """
        self.n = n
        self. l = l
        self.u = u
        self.generate()

    def generate(self):
        self.mat = sparse.rand(self.n, self.n, .5).todense()

        # print "\n\nOriginal: \n", self
        for row in xrange(self.n - self.u - 1):
            self.mat[row, (row+1 + self.u):] = 0

        # print "\n\nAfter upper fix:\n", self
        for row in xrange(self.l+1, self.n):
            self.mat[row, :(row - self.l)] = 0

        # print "\n\nAfter lower fix:\n", self
        return BandMatrix(np.array(self.mat), self.l, self.u)

    def test_zeros(self):
        for row in xrange(self.mat.shape[0]):
            for col in xrange(row - self.l):
                assert not self.mat[row, col]

            for col in xrange(row + self.u + 1, self.mat.shape[1]):
                assert not self.mat[row,col]

    def raw(self):
        return np.array(self.data)

    def __repr__(self,):
        return self.mat.__repr__()

def test():
    """
    This tests that all the zero regions are indeed zero
    """
    print "Test 1:",
    mat = GenBand(6, 2, 3)
    mat.test_zeros()
    print "Success!"

    print "Test 2:",
    mat = GenBand(10, 2, 4)
    mat.test_zeros()
    print "Success!"

    print "Test 3:",
    mat = GenBand(50, 10, 15)
    mat.test_zeros()
    print "Success!"

    print "Test 4:",
    mat = GenBand(100, 20, 35)
    mat.test_zeros()
    print "Success!"

    print "Test 5:",
    mat = GenBand(1000, 40, 500)
    mat.test_zeros()
    print "Success!"

if __name__ == "__main__":
    test()
