#! /usr/bin/env python

from __future__ import print_function, division
from openturns import *

TESTPREAMBLE()
RandomGenerator.SetSeed(0)

# Default constructor
myDefautModel = UserDefinedCovarianceModel()
print("myDefautModel = ", myDefautModel)

# Default dimension parameter to evaluate the model
dimension = 1
spatialDimension = 1

# Amplitude values
amplitude = NumericalPoint(dimension)
# Scale values
scale = NumericalPoint(dimension)
# Spatial correclation
spatialCorrelation = CorrelationMatrix(dimension)
for index in range(dimension):
    # constant amplitude
    amplitude[index] = 1.0
    scale[index] = (index + 1.0) / dimension

# Sample a ExponentialModel
referenceModel = ExponentialModel(
    scale, amplitude, spatialCorrelation)

size = 20
timeGrid = RegularGrid(0.0, 0.1, size)
covarianceCollection = CovarianceMatrixCollection((size * (size + 1)) // 2)

k = 0
for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    for j in range(i + 1):
        s = timeGrid.getValue(j)
        covarianceCollection[k] = referenceModel(t, s)
        k = k + 1
# Create a UserDefinedCovarianceModel
myModel = UserDefinedCovarianceModel(timeGrid, covarianceCollection)
print("myModel=", myModel)

for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    for j in range(timeGrid.getN()):
        s = timeGrid.getValue(j)
        # We look for cov(s,t) ==> when adding to the collection, we compute cov(t,s)
        # Because of symmetry, we check the right index computation
        print("myModel =  %.6g" % myModel(s, t)[
              0, 0], ", referenceModel=  %.6g" % referenceModel(s, t)[0, 0])
print("myModel.discretize()=", myModel.discretize(timeGrid))
