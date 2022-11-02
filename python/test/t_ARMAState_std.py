#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Constructor with size
dim = 1
p = 6
q = 4

lastValues = ot.Sample(p, dim)
lastNoiseValues = ot.Sample(q, dim)

for j in range(dim):
    # Fill the AR-part (the last p-coefficients X_{-1}, X{-2},..., X_{-p})
    for i in range(p):
        lastValues[i, j] = 2.0 * i + 3.0 * j + 1.0

    # Fill the MA-part (the last p-coefficients \epsilon_{-1},
    # \epsilon_{-2},..., \epsilon_{-p})
    for i in range(q):
        lastNoiseValues[i, j] = ot.RandomGenerator.Generate()

# Print the initial state of the ARMA : coefficients
print("Last values = ", repr(lastValues))
print("Last noise values = ", repr(lastNoiseValues))

# default constructor
myDefaultState = ot.ARMAState()

print("ARMAState with default constructor = ", repr(myDefaultState))

# standard constructor
myState = ot.ARMAState(lastValues, lastNoiseValues)
print("ARMAState  = ", repr(myState))
