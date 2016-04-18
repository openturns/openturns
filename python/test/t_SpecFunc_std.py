#!/usr/bin/env python

from __future__ import print_function
import openturns as ot

for lx in range(10):
    x = 2**lx
    print('x=', x, 'log2=', ot.SpecFunc.Log2(x))

for lx in range(10):
    x = 2**lx - 1
    print('x=', x, 'bits=', ot.SpecFunc.BitCount(x))

for x in [0, 1, 2, 3, 4, 10, 100, 1000]:
    print('x=', x, 'nextpow2=', ot.SpecFunc.NextPowerOfTwo(x))
