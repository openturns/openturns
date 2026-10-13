#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

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

# Extra coverage: error branches, accessors, symmetry, persistence

with ott.assert_raises(Exception):
    ot.UserDefinedStationaryCovarianceModel(
        timeGrid, ot.SquareMatrixCollection(3)
    )
mismatched = ot.SquareMatrixCollection(size)
for i in range(size):
    mismatched[i] = ot.SquareMatrix(1)
mismatched[3] = ot.SquareMatrix(2)
with ott.assert_raises(Exception):
    ot.UserDefinedStationaryCovarianceModel(timeGrid, mismatched)
with ott.assert_raises(Exception):
    myModel.computeAsScalar([0.0, 1.0])
with ott.assert_raises(Exception):
    myModel([0.0, 1.0])
with ott.assert_raises(Exception):
    modelSingle.computeAsScalar([0.5])
# Negative shifts use the mirrored grid point
ott.assert_almost_equal(myModel([-0.5])[0, 0], myModel([0.5])[0, 0], 1e-12, 1e-12)
ott.assert_almost_equal(
    myModel.computeAsScalar([-0.5]), myModel.computeAsScalar([0.5]), 1e-12, 1e-12
)
assert myModel == ot.UserDefinedStationaryCovarianceModel(
    timeGrid, covarianceCollection
)
assert not (myModel != ot.UserDefinedStationaryCovarianceModel(
    timeGrid, covarianceCollection
))
assert myModel != modelSingle
assert myModel.getTimeGrid() == timeGrid
assert len(repr(myModel)) > 0
assert len(str(myModel)) > 0
# discretize follows the operator() path for non-scalar output
discretized = modelSingle.discretize(ot.Sample([[0.0], [0.5]]))
assert discretized.getDimension() == 4
ott.assert_almost_equal(discretized[0, 0], covariance[0, 0], 1e-12, 1e-12)
study = ot.Study()
study.setStorageManager(
    ot.XMLStorageManager("t_UserDefinedStationaryCovarianceModel_std_tmp.xml")
)
study.add("model", myModel)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(
    ot.XMLStorageManager("t_UserDefinedStationaryCovarianceModel_std_tmp.xml")
)
loadedStudy.load()
loadedModel = ot.UserDefinedStationaryCovarianceModel()
loadedStudy.fillObject("model", loadedModel)
assert loadedModel == myModel
os.remove("t_UserDefinedStationaryCovarianceModel_std_tmp.xml")
