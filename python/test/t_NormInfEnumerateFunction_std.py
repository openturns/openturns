#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

stratas = 4
for dimension in range(1, 4):
    f = ot.NormInfEnumerateFunction(dimension)
    size = f.getStrataCumulatedCardinal(stratas)
    print("First", size, "values for dimension", dimension)
    for i in range(size):
        print('i=', i, 'f(i)=', f(i))

# with bounds
f = ot.NormInfEnumerateFunction(2)
f.setUpperBound([int(1e8), 3])
for index in range(25):
    m = f(index)
    index_inv = f.inverse(m)
    print("index=", index, repr(m))
    assert m[1] <= 3, "wrong bound"
    assert index == index_inv, "wrong inverse"
f = ot.NormInfEnumerateFunction(2)
f.setUpperBound([3, int(1e8)])
for index in range(25):
    m = f(index)
    index_inv = f.inverse(m)
    print("index=", index, repr(m), index_inv)
    assert m[0] <= 3, "wrong bound"
    assert index == index_inv, "wrong inverse"
