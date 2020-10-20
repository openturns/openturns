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

for n in [4, 5, 17]:
    for k in [2, 0, 7, 3]:
        if k <= n:
            print('n=', n, 'k=', k, 'binom=',
                  ot.SpecFunc.BinomialCoefficient(n, k))

for x in [-2.0, -1.0, 0.0, 1.0, 2.0]:
    for n in [0, 1, 2]:
        print('x=', x, 'n=', n, 'ipow=',  ot.SpecFunc.IPow(x, n))

for x in [-4.0, 4.0, 9.0]:
    for n in [1, 2]:
        if x < 0.0 and n % 2 == 0:
            continue
        print('x=', x, 'n=', n, 'iroot=',  ot.SpecFunc.IRoot(x, n))
