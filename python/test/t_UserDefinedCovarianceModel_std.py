#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

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
referenceModel = ot.ExponentialModel(scale, amplitude, spatialCorrelation)

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
myModel2 = ot.UserDefinedCovarianceModel(timeGrid, referenceModel.discretize(timeGrid))
print("myModel2=", myModel2)

for i in range(timeGrid.getN()):
    t = timeGrid.getValue(i)
    for j in range(timeGrid.getN()):
        s = timeGrid.getValue(j)
        # We look for cov(s,t) ==> when adding to the collection, we compute cov(t,s)
        # Because of symmetry, we check the right index computation
        print(
            "myModel =  %.6g" % myModel(s, t)[0, 0],
            ", referenceModel=  %.6g" % referenceModel(s, t)[0, 0],
        )
print("myModel.discretize()=", myModel.discretize(timeGrid))

# Test the drawing method as a nonstationary model, in the covariance/correlation range
for correlationFlag in [False, True]:
    graph = myModel.draw(0, 0, 0.0, 2.0, 21, False, correlationFlag)
    assert "Contour" in repr(graph)
    drawable = graph.getDrawable(0)
    levels = drawable.getLevels()
    assert len(levels) > 0, "expected non-empty contour levels"
    data = drawable.getData()
    dataMin = data.getMin()[0]
    dataMax = data.getMax()[0]
    assert dataMax > dataMin
    for level in levels:
        assert dataMin < level < dataMax, "bad level bounds"

# Plateau data leaves no interior level: check mid-value fallback
plateau = ot.Contour(2, 2, [[0.0], [1.0], [0.0], [1.0]])
levels = plateau.getLevels()
assert len(levels) == 1, "expected single mid-value level"
assert levels[0] == 0.5
plateau.setIsFilled(True)
levels = plateau.getLevels()
assert len(levels) == 2, "filled contours require at least two levels"
for level in levels:
    assert 0.0 < level < 1.0
filled = ot.Contour(2, 2, [[0.0], [1.0], [0.0], [1.0]])
filled.setIsFilled(True)
filled.buildDefaultLevels()
assert len(filled.getLevels()) == 2, "filled needs two levels"

# Extra coverage: accessors, error branches, save/load (no prints to keep expout stable)

assert isinstance(repr(myModel), str)
assert isinstance(str(myModel), str)
assert myModel == myModel
assert not (myModel != myModel)
assert myModel == ot.UserDefinedCovarianceModel(timeGrid, covariance)
assert myModel != myDefaultModel
assert myModel.getMesh().getVerticesNumber() == size
assert myModel.getTimeGrid().getN() == size
# discretize on the defining mesh returns the full covariance
assert myModel.discretize(timeGrid) == covariance
# discretize on other vertices projects on nearest neighbours
otherVertices = ot.Sample([[0.05], [0.15]])
assert myModel.discretize(otherVertices).getDimension() == 2
assert myModel.discretizeRow(otherVertices, 0).getSize() == 2
assert myModel.discretizeAndFactorize(timeGrid.getVertices()).getDimension() == size
# single-vertex mesh takes the N == 1 branch
singleGrid = ot.RegularGrid(0.0, 0.1, 1)
singleModel = ot.UserDefinedCovarianceModel(singleGrid, ot.CovarianceMatrix(1))
ott.assert_almost_equal(singleModel.computeAsScalar([0.0], [0.0]), 1.0, 1e-12, 1e-12)
assert singleModel([0.0], [0.0]).getDimension() == 1
# error branches
with ott.assert_raises(Exception):
    ot.UserDefinedCovarianceModel(timeGrid, ot.CovarianceMatrix(7))
with ott.assert_raises(Exception):
    myModel.computeAsScalar([0.0, 1.0], [0.0])
with ott.assert_raises(Exception):
    myModel.computeAsScalar([0.0], [0.0, 1.0])
with ott.assert_raises(Exception):
    myModel([0.0, 1.0], [0.0])
# multi-output model: scalar and row accessors must raise
bigCovariance = ot.CovarianceMatrix(2 * 3)
for i in range(6):
    bigCovariance[i, i] = 1.0
multiModel = ot.UserDefinedCovarianceModel(ot.Mesh(ot.RegularGrid(0.0, 0.1, 3)), bigCovariance)
assert multiModel.getOutputDimension() == 2
with ott.assert_raises(Exception):
    multiModel.computeAsScalar([0.0], [0.1])
with ott.assert_raises(Exception):
    multiModel.discretizeRow(multiModel.getMesh().getVertices(), 0)
# save/load round-trip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("userdefinedcovariancemodel.xml"))
study.add("model", myModel)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("userdefinedcovariancemodel.xml"))
loadedStudy.load()
assert loadedStudy.hasObject("model")

os.remove("userdefinedcovariancemodel.xml")
