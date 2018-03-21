#! /usr/bin/env python

from __future__ import print_function, division
from openturns import *

TESTPREAMBLE()


dimension = 2
inVar = Description(dimension)
inVar[0] = "x1"
inVar[1] = "x2"
formula = Description(1)
formula[0] = "x1^3+1.5*x2^3-x1*x2"
model = SymbolicFunction(inVar, formula)
coll = []
formula[0] = "x1"
coll.append(SymbolicFunction(inVar, formula))
formula[0] = "x2"
coll.append(SymbolicFunction(inVar, formula))
formula[0] = "x1^2"
coll.append(SymbolicFunction(inVar, formula))
formula[0] = "x2^2"
coll.append(SymbolicFunction(inVar, formula))
basis = Basis(coll)
indices = list(range(len(coll)))
size = 5
inputSample = Sample(size * size, dimension)
weight = Point(inputSample.getSize(), 1)
for i in range(inputSample.getSize()):
    inputSample[i] = Point(
        (float(i % size) / size, (1.0 * (i // size)) / size))
    weight[i] = (i % size + 1) * ((i // size) + 1)
penalizationFactor = 0.25
# Uniform weight, no penalization
algo = PenalizedLeastSquaresAlgorithm(
    inputSample, model(inputSample), Point(inputSample.getSize(), 1.0), coll, indices)
print("Uniform weight, no penalization")
print("Coefficients=", (algo.getCoefficients()))
print("Residual=%g" % algo.getResidual())
print("RelativeError=%g" % algo.getRelativeError())
# Uniform weight, spherical penalization
algo = PenalizedLeastSquaresAlgorithm(
    inputSample, model(inputSample), Point(inputSample.getSize(), 1.0), coll, indices, penalizationFactor)
print("Uniform weight, spherical penalization")
print("Coefficients=", (algo.getCoefficients()))
print("Residual=%g" % algo.getResidual())
# Non uniform weight, spherical penalization
algo = PenalizedLeastSquaresAlgorithm(
    inputSample, model(inputSample), weight, coll, indices)
print("Non uniform weight, no penalization")
print("Coefficients=", (algo.getCoefficients()))
print("Residual=%g" % algo.getResidual())
algo = PenalizedLeastSquaresAlgorithm(
    inputSample, model(inputSample), weight, coll, indices, penalizationFactor)
print("Non uniform weight, spherical penalization")
print("Coefficients=", (algo.getCoefficients()))
print("Residual=%g" % algo.getResidual())
penalizationMatrix = CovarianceMatrix(4)
for i in range(4):
    penalizationMatrix[i, i] = 1.0
for i in range(3):
    penalizationMatrix[i, i + 1] = 1.0 / 8.0
algo = PenalizedLeastSquaresAlgorithm(
    inputSample, model(inputSample), weight, coll, indices, penalizationFactor, penalizationMatrix)
print("Non uniform weight, non spherical penalization")
print("Coefficients=", (algo.getCoefficients()))
print("Residual=%g" % algo.getResidual())
