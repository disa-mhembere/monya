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
#

# BandMatrix.py
# Created by Disa Mhembere on 2017-10-29.
# Email: disa@jhu.edu
# NOTE: This Banded matrix is inefficient, but identical to the format
#   used by BLAS: http://www.netlib.org/lapack/lug/node124.html
# NOTE: There is a partitioning that is imposed by the multiplication
#   scheme.

import numpy as np

class BandMatrix(object):

    def __init__(self, mat, kl, ku):
        self.m, self.n = mat.shape # nrow, ncol
        self.l = self.u = 0 # FIXME: Compute efficiently

        self.nrow = ku + kl + 1 # nrow in Banded matrix not orig

        self.data = np.zeros((kl+ku+1, self.n))
        self._build_banded_(mat)

    def _build_banded_(self, mat):

        # bottom up
        drow = 0
        for diag_index in range(-self.n+1, self.n)[::-1]:
            d = mat.diagonal(diag_index)

            if diag_index == 0:
                drow += 1
            elif d.any():
                pad_len = self.n - len(d)
                if diag_index > 0:
                    self.u += 1
                    d = np.pad(d, (pad_len,0), "constant",\
                            constant_values=0)
                elif diag_index < 0:
                    self.l += 1
                    d = np.pad(d, (0, pad_len), "constant",\
                            constant_values=0)

                self.data[drow] = d
                drow += 1

    def __mul__(self, b):
        """
        b is a vector
        """

        pass

    def __getitem__(self, index):
        row, col = index
        # FIXME: Potentiall expensive
        if row >= max(1, col - self.u) and row <= min(self.m, col+self.l):
            return self.data[self.u+row-col, col]
        else:
            return 0

    def __setitem__(self, index):
       self.data[self.u+row-col, col]

    def __repr__(self):
        return "Banded: Superdiagonals (ku) = {}, Subdiagonals (kl) = {}\n".format(self.u, self.l) +\
                self.data.__repr__()

