#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# 2 D case

# Matrices of the process
dim = 2
squareMatrix1 = ot.SquareMatrix(dim)
squareMatrix1[0, 0] = 0.2
squareMatrix1[1, 0] = 0.3
squareMatrix1[0, 1] = 0.7
squareMatrix1[1, 1] = 0.4

# Second matrix to add to the ARMACoefficients
squareMatrix2 = ot.SquareMatrix(dim)
squareMatrix2[0, 0] = 0.1
squareMatrix2[1, 0] = 0.
squareMatrix2[0, 1] = 0.
squareMatrix2[1, 1] = 0.5

# ARMA(p, q)
p = 1
q = 1

# AR coefficients
coefficientsP = ot.ARMACoefficients(p, dim)
coefficientsP[0] = squareMatrix1

# MA coefficients
coefficientsQ = ot.ARMACoefficients(p, dim)
coefficientsQ[0] = squareMatrix2

print("coefficientsP = ", repr(coefficientsP))
print("coefficientsQ = ", repr(coefficientsQ))

# Time grid creation and White Noise
Tmin = 0.0
deltaT = 0.1
steps = 11

# Initialization of the TimeGrid timeGrid1
timeGrid = ot.RegularGrid(Tmin, deltaT, steps)

# Distributions for the  choice
dist1 = ot.Normal(0.0, 0.01)
dist2 = ot.Normal(0.0, 0.02)

# Create a collection of distribution
aCollection = [dist1, dist2]

dist = ot.ComposedDistribution(aCollection)
print("dist = ",  dist)

epsilon = ot.WhiteNoise(dist)

# Setting the timeGrid
epsilon.setTimeGrid(timeGrid)

# Last coefficients values
lastValues = ot.Sample(p, dim)
lastNoiseValues = ot.Sample(q, dim)

#
for j in range(dim):
    # Fill the AR-part (the last p-coefficients X_{-1}, X{-2},..., X_{-p})
    for i in range(p):
        lastValues[i, j] = ot.RandomGenerator.Generate()

    # Fill the MA-part (the last p-coefficients \epsilon_{-1},
    # \epsilon_{-2},..., \epsilon_{-p})
    for i in range(q):
        lastNoiseValues[i, j] = ot.RandomGenerator.Generate()

# Print the initial state of the ARMA : coefficients
print("Last values of the process = ",  lastValues)
print("Last innovations of the process = ",  lastNoiseValues)

# ARMAState creation
# instanciation of timeGrid
state = ot.ARMAState(lastValues, lastNoiseValues)
process = ot.Process(ot.ARMA(coefficientsP, coefficientsQ, epsilon))
process2 = ot.ARMA(coefficientsP, coefficientsQ, epsilon)
process3 = ot.ARMA(coefficientsP, coefficientsQ, epsilon, state)
print("process = ",  process)
print("ARMA process = ",  process2)
print("ARMA process with ARMAstate = ",  process3)

# Test realization
print("One realization=", process2.getRealization())

# Some steps further
stepNumber = 4
print("One future=", process2.getFuture(stepNumber))
size = 3
print("Some futures=", process2.getFuture(stepNumber, size))

