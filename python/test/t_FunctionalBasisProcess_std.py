#! /usr/bin/env python

from __future__ import print_function
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

try:
    Tmin = 0.0
    deltaT = 0.1
    N = 11

    # Initialization of the RegularGrid timeGrid1
    timeGrid = RegularGrid(Tmin, deltaT, N)

    # Functional basis
    basisDimension = 10
    basis = []
    coefficients = DistributionCollection(basisDimension)

    for i in range(basisDimension):
        basis.append(SymbolicFunction("x", "sin(" + str(i) + "*x)"))
        coefficients[i] = Normal(0.0, (1.0 + i))

    process = FunctionalBasisProcess(
        ComposedDistribution(coefficients), Basis(basis))

    # Setting the timeGrid
    process.setTimeGrid(timeGrid)

    # Print of the process
    print("process = ", process)

    # Initialization of the TimeSeries
    timeSerie = process.getRealization()

    # Full prints
    print("timeSerie = ", timeSerie)

    # Some steps further
    stepNumber = 4
    print("One future=", process.getFuture(stepNumber))
    size = 3
    print("Some futures=", process.getFuture(stepNumber, size))

except:
    import sys
    print("t_FunctionalBasisProcess_std.py",
          sys.exc_info()[0], sys.exc_info()[1])
