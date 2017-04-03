#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    # Constructor with size
    dim = 1
    p = 6
    q = 4

    lastValues = Sample(p, dim)
    lastNoiseValues = Sample(q, dim)

    for j in range(dim):
        # Fill the AR-part (the last p-coefficients X_{-1}, X{-2},..., X_{-p})
        for i in range(p):
            lastValues[i, j] = 2.0 * i + 3.0 * j + 1.0

        # Fill the MA-part (the last p-coefficients \epsilon_{-1},
        # \epsilon_{-2},..., \epsilon_{-p})
        for i in range(q):
            lastNoiseValues[i, j] = RandomGenerator.Generate()

    # Print the initial state of the ARMA : coefficients
    print("Last values = ", repr(lastValues))
    print("Last noise values = ", repr(lastNoiseValues))

    # default constructor
    myDefaultState = ARMAState()

    print("ARMAState with default constructor = ", repr(myDefaultState))

    # standard constructor
    myState = ARMAState(lastValues, lastNoiseValues)
    print("ARMAState  = ", repr(myState))

except:
    import sys
    print("t_ARMAState_std.py", sys.exc_info()[0], sys.exc_info()[1])
