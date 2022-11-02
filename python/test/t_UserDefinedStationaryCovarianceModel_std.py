#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()


# Default constructor
myDefaultModel = ot.UserDefinedStationaryCovarianceModel()
print("myDefaultModel = ", myDefaultModel)

# Default dimension parameter to evaluate the model
dimension = 1
inputDimension = 1

# Amplitude values
amplitude = ot.Point(dimension)
# Scale values
scale = ot.Point(dimension)
# Spatial correclation
spatialCorrelation = ot.CorrelationMatrix(dimension)
for index in range(dimension):
    # constant amplitude
    amplitude[index] = 2.0
    scale[index] = (index + 1.0) / dimension

# Sample a ExponentialModel
referenceModel = ot.ExponentialModel(scale, amplitude, spatialCorrelation)

print("reference model=", referenceModel)
size = 100
timeGrid = ot.RegularGrid(0.0, 0.1, size)
covarianceCollection = ot.SquareMatrixCollection(size)

for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    covarianceCollection[i] = referenceModel(t)
# Create a UserDefinedStationaryCovarianceModel
myModel = ot.UserDefinedStationaryCovarianceModel(timeGrid, covarianceCollection)
print("myModel=", myModel)

for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    # We look for cov(s,t) ==> when adding to the collection, we compute cov(t,s)
    # Because of symmetry, we check the right index computation
    print(
        "t= %.6g" % t,
        "myModel =  %.6g" % myModel(t)[0, 0],
        ", referenceModel=  %.6g" % referenceModel(t)[0, 0],
    )

# Test the drawing method as a nonstationary model, in the covariance range
graph = myModel.draw(0, 0, -2.0, 2.0, 21, True, False)
print(graph)
# Test the drawing method as a nonstationary model, in the correlation
# range
graph = myModel.draw(0, 0, -2.0, 2.0, 21, True, True)
print(graph)
