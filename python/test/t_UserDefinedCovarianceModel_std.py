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
inputDimension = 1

# Amplitude values
amplitude = Point(dimension)
# Scale values
scale = Point(dimension)
# Spatial correlation
spatialCorrelation = CorrelationMatrix(dimension)
for index in range(dimension):
    # constant amplitude
    amplitude[index] = 2.0
    scale[index] = (index + 1.0) / dimension

# Sample an ExponentialModel
referenceModel = ExponentialModel(
    scale, amplitude, spatialCorrelation)

size = 20
timeGrid = RegularGrid(0.0, 0.1, size)
covariance = CovarianceMatrix(size)

for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    for j in range(i + 1):
        s = timeGrid.getValue(j)
        covariance[i, j] = referenceModel.computeAsScalar([t], [s])
# Create a UserDefinedCovarianceModel
myModel = UserDefinedCovarianceModel(timeGrid, covariance)
print("myModel=", myModel)
myModel2 = UserDefinedCovarianceModel(
    timeGrid, referenceModel.discretize(timeGrid))
print("myModel2=", myModel2)

for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    for j in range(timeGrid.getN()):
        s = timeGrid.getValue(j)
        # We look for cov(s,t) ==> when adding to the collection, we compute cov(t,s)
        # Because of symmetry, we check the right index computation
        print("myModel =  %.6g" % myModel(s, t)[
              0, 0], ", referenceModel=  %.6g" % referenceModel(s, t)[0, 0])
print("myModel.discretize()=", myModel.discretize(timeGrid))

# Test the drawing method as a nonstationary model, in the covariance range
graph = myModel.draw(0, 0, 0.0, 2.0, 21, False, False)
print(graph)
# Test the drawing method as a nonstationary model, in the correlation range
graph = myModel.draw(0, 0, 0.0, 2.0, 21, False, True)
print(graph)
