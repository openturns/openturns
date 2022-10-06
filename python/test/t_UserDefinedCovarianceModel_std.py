#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

# Default constructor
myDefaultModel = ot.UserDefinedCovarianceModel()
print("myDefaultModel = ", myDefaultModel)

# Default dimension parameter to evaluate the model
dimension = 1
inputDimension = 1

# Amplitude values
amplitude = ot.Point(dimension)
# Scale values
scale = ot.Point(dimension)
# Spatial correlation
spatialCorrelation = ot.CorrelationMatrix(dimension)
for index in range(dimension):
    # constant amplitude
    amplitude[index] = 2.0
    scale[index] = (index + 1.0) / dimension

# Sample an ExponentialModel
referenceModel = ot.ExponentialModel(
    scale, amplitude, spatialCorrelation)

size = 20
timeGrid = ot.RegularGrid(0.0, 0.1, size)
covariance = ot.CovarianceMatrix(size)

for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    for j in range(i + 1):
        s = timeGrid.getValue(j)
        covariance[i, j] = referenceModel.computeAsScalar([t], [s])
# Create a UserDefinedCovarianceModel
myModel = ot.UserDefinedCovarianceModel(timeGrid, covariance)
print("myModel=", myModel)
myModel2 = ot.UserDefinedCovarianceModel(
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
