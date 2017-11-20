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

class GenSparse():
    def __init__(self, n, density):
        """
        @param n: the number of rows/cols in the square matrix
        @param density: probability of an entry
        """
        self.n = n
        self.density = density
        self.generate()

    def generate(self):
        self.mat = sparse.rand(self.n, self.n, self.density)

    def raw(self):
        return self.mat

    def write(self, filename, fmt="coo"):
        if fmt == "coo":
            ret = np.append(np.append(np.append(np.array([self.n, len(self.mat.row)]),
                    self.mat.row), self.mat.col), self.mat.data)
            ret = ret.astype(np.float64).tofile(filename)
        else:
            raise NotImplementedError("Only coo for now")

    def __repr__(self,):
        return self.mat.__repr__()

def test():
    np.set_printoptions(3, linewidth=100)

    print "Test 1:",
    mat = GenSparse(10, .3)
    print mat
    mat.write("coo_n10_m30.bin")
    print "Success!"

if __name__ == "__main__":
    test()
