#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


size = 10
for dimension in range(1, 4):
    f = ot.LinearEnumerateFunction(dimension)
    print("First", size, " values for dimension", dimension)
    for index in range(size):
        multiIndex = f(index)
        inverse = f.inverse(multiIndex)
        print("index=", index, ", multi-index=", multiIndex, ", linear index=", inverse)
        assert inverse == index, "no bijection"
    for d in range(10):
        idx = f.getMaximumDegreeStrataIndex(d)
        print("degree", d, "max_degree_strata_index", idx)
        size = f.getBasisSizeFromTotalDegree(d)
        print("degree", d, "size", size)
#
print("Test getMarginal() from Indices")
f = ot.LinearEnumerateFunction(5)
marginalf = f.getMarginal([0, 3, 4])
for index in range(10):
    m = marginalf(index)
    index_inv = marginalf.inverse(m)
    print("index=", index, repr(m), index_inv)

print("Test getMarginal() from a single integer")
f = ot.LinearEnumerateFunction(5)
marginalf = f.getMarginal(3)
for index in range(10):
    m = marginalf(index)
    index_inv = marginalf.inverse(m)
    print("index=", index, repr(m), index_inv)
