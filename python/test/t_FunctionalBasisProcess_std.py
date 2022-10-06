#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

Tmin = 0.0
deltaT = 0.1
N = 11

# Initialization of the RegularGrid timeGrid1
timeGrid = ot.RegularGrid(Tmin, deltaT, N)

# Functional basis
basisDimension = 10
basis = []
coefficients = ot.DistributionCollection(basisDimension)

for i in range(basisDimension):
    basis.append(ot.SymbolicFunction("x", "sin(" + str(i) + "*x)"))
    coefficients[i] = ot.Normal(0.0, (1.0 + i))

process = ot.FunctionalBasisProcess(
    ot.ComposedDistribution(coefficients), ot.Basis(basis))

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

