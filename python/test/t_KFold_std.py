#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    size = 100
    xuniform = Uniform(0.9, 1.1)
    x = xuniform.getSample(size)
    yuniform = Uniform(1.9, 2.1)
    y = yuniform.getSample(size)
    w = [1.0] * size
    f = SymbolicFunction(['x'], ['2.0*x'])
    basis = [f]
    indices = list(range(len(basis)))

    fittingAlgo = KFold()

    print("algo =", fittingAlgo)

    result = fittingAlgo.run(x, y, w, basis, indices)

    print("result = %g" % result)


except:
    import sys
    print("t_KFold_std.py", sys.exc_info()[0], sys.exc_info()[1])
