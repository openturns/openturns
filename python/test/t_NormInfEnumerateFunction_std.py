#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.TESTPREAMBLE()

stratas = 4
for dimension in range(1, 4):
    f = ot.NormInfEnumerateFunction(dimension)
    size = f.getStrataCumulatedCardinal(stratas)
    print("First", size, "values for dimension", dimension)
    for i in range(size):
        print('i=', i, 'f(i)=', f(i))
