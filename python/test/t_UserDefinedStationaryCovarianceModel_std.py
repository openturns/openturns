#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

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

# Constructor from a single covariance matrix, see issue #1243
covariance = ot.SquareMatrix(2)
covariance[0, 0] = 1.0
covariance[1, 1] = 2.0
covariance[0, 1] = 0.3
covariance[1, 0] = 0.3
modelSingle = ot.UserDefinedStationaryCovarianceModel(timeGrid, covariance)
assert modelSingle.getOutputDimension() == 2, "output dimension"

for tau in (0.5, 1.5, 3.0):
    value = modelSingle(tau)[0, 0]
    assert abs(value - covariance[0, 0]) < 1e-12, "single matrix value"
# consistency with the collection constructor filled with a constant
constantCollection = ot.SquareMatrixCollection(timeGrid.getN())
for i in range(timeGrid.getN()):
    constantCollection[i] = covariance
modelCollection = ot.UserDefinedStationaryCovarianceModel(
    timeGrid, constantCollection
)
for i in range(0, timeGrid.getN(), 7):
    tau = timeGrid.getValue(i)
    diff = modelSingle(tau) - modelCollection(tau)
    ott.assert_almost_equal(diff[0, 0], 0.0, 1e-12, 1e-12)
    ott.assert_almost_equal(diff[1, 1], 0.0, 1e-12, 1e-12)
