#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:

    dim = 2
    distribution = Normal(dim)
    #
    # Case 1: composite random vector based event
    #

    # The input vector
    X = RandomVector(distribution)
    # The model: the identity function
    inVars = Description(dim)
    for i in range(dim):
        inVars[i] = "x" + str(i)
    model = SymbolicFunction(inVars, inVars)
    # The output vector
    Y = CompositeRandomVector(model, X)
    # The domain: [0, 1]^dim
    domain = Interval(dim)
    # The event
    event = Event(Y, domain)

    print("sample=")
    print(event.getSample(10))

    #
    # Case 2: process based event
    #

    # The input process
    X = WhiteNoise(distribution)
    # The domain: [0, 1]^dim
    domain = Interval(dim)
    # The event
    event = Event(X, domain)

    print("sample=")
    print(event.getSample(10))


except:
    import sys
    print("t_Event_domain.py", sys.exc_info()[0], sys.exc_info()[1])
