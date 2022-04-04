#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()


size = 10
for dimension in range(1, 4):
    f = ot.LinearEnumerateFunction(dimension)
    print("First", size, " values for dimension", dimension)
    for index in range(size):
        multiIndex = f(index)
        inverse = f.inverse(multiIndex)
        print("index=", index, ", multi-index=", multiIndex,
              ", linear index=", inverse)
        assert inverse == index, "no bijection"
    for d in range(10):
        idx = f.getMaximumDegreeStrataIndex(d)
        print('degree', d, 'max_degree_strata_index', idx)
        size = f.getBasisSizeFromTotalDegree(d)
        print('degree', d, 'size', size)
