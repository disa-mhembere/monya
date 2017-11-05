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
# NOTE: This Banded matrix implementation is inefficient,
#       but identical to the format
#   used by BLAS: http://www.netlib.org/lapack/lug/node124.html
# NOTE: There is a partitioning that is imposed by the multiplication
#   scheme.

import numpy as np

np.set_printoptions(4, linewidth=100)

class BandMatrix(object):

    def __init__(self, mat, kl, ku, store="R"):
        """
        mat: a numpy ndarray
        kl: number of subdiagonals (nnz diags beneath main)
        ku: number of superdiagonals (nnz diags above main)
        store: Row ("R") or Column ("C") storage
        """
        self.m, self.n = mat.shape # nrow, ncol originially
        self.l = self.u = 0 # FIXME: Compute efficiently

        self.store = store.upper()
        if store == "C":
            self.nrow = ku + kl + 1 # nrow in Banded matrix not orig
            self.ncol = self.n
            self.data = np.zeros((kl+ku+1, self.n))
        elif store == "R":
            self.nrow = len(mat.diagonal())
            self.ncol = ku + kl + 1
            self.data = np.zeros((len(mat.diagonal()), kl+ku+1))
        else:
            raise RuntimeError(
                    "Unknown storage type {}".format(self.store))

        self._build_banded_(mat)

    def _build_banded_(self, mat):
        if self.store == "C":
            self._build_col_store_(mat)
        elif self.store == "R":
            self._build_row_store_(mat)
        else:
            assert False, "Unknown store type {}".format(self.store)

    def _build_row_store_(self, mat):
        dcol = 0
        for diag_index in range(-self.nrow+1, self.nrow):
            d = mat.diagonal(diag_index)

            if diag_index == 0:
                if d.any():
                    self.data[:,dcol] = d
                dcol += 1
            elif d.any():
                pad_len = self.n - len(d) # FIXME
                if diag_index > 0:
                    self.l += 1
                    d = np.pad(d, (0, pad_len), "constant",\
                            constant_values=0)
                elif diag_index < 0:
                    self.u += 1
                    d = np.pad(d, (pad_len, 0), "constant",\
                            constant_values=0)

                self.data[:,dcol] = d
                dcol += 1

    def _build_col_store_(self, mat):
        # BLAS
        # top right hand corner down build up
        drow = 0
        for diag_index in range(-self.n+1, self.n)[::-1]:
            d = mat.diagonal(diag_index)

            if diag_index == 0:
                if d.any():
                    self.data[drow] = d
                drow += 1
            elif d.any():
                pad_len = self.n - len(d)
                if diag_index > 0:
                    self.u += 1
                    # pad left (front)
                    d = np.pad(d, (pad_len,0), "constant",\
                            constant_values=0)
                elif diag_index < 0:
                    self.l += 1
                    # pad right (end)
                    d = np.pad(d, (0, pad_len), "constant",\
                            constant_values=0)

                self.data[drow] = d
                drow += 1

    def __mul__(self, x):
        """
        x: is a vector
        """

        # According to: http://www.cs.rpi.edu/~flaherje/pdf/lin2.pdf
        # Changes for 0-based indexing
        if self.store == "R":
            res = np.zeros(self.n)
            for i in xrange(self.n):
                jstart = max(0, i - self.l) # 0: Extend range
                start = jstart - i + self.l
                jstop = min(self.n, i + self.u + 1) # +1: Extend range
                stop = jstop - i + self.l

                assert jstart < jstop and start < stop, "Start - Stop"
                assert jstart >= 0 and start >= 0, "xstart < 0"
                assert jstop <= self.nrow and start <= self.nrow, "xstart < 0"

                res[i] = np.dot((self.data[i, start:stop]).T,\
                        x[jstart:jstop])
            return res.reshape(self.n,1)
        else:
            raise NotImplementedError("C storage matrix vector multiply")

    def __getitem__(self, index):
        row, col = index
        # FIXME: Potentially expensive
        if self.store == "C":
            if row >= max(1, col - self.u) and \
                    row <= min(self.m, col+self.l):
                return self.data[self.u+row-col, col]
            else:
                return 0
        elif self.store == "R":
            b_col = self.l + col - row # column mapping
            if b_col < 0 or b_col > (self.l + self.u):
                return 0
            else:
                return self.data[row, b_col]
        else:
            assert 0

    def __setitem__(self, index):
       self.data[self.u+row-col, col]

    def __repr__(self):
        return "Banded: Superdiagonals (ku) = {}, Subdiagonals (kl) = {}, Store = '{}'\n".format(self.u, self.l, self.store) +\
                self.data.__repr__()

