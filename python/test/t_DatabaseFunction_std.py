#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Database construction
dimension = 2

inputSample = ot.Sample(0, dimension)
inputSample.add(ot.Point(dimension, 1.0))
inputSample.add(ot.Point(dimension, 2.0))
inputSample.setDescription(["x0", "x1"])
outputSample = ot.Sample(0, 1)
outputSample.add(ot.Point(1, 4.0))
outputSample.add(ot.Point(1, 5.0))
outputSample.setDescription(["y0"])
database = ot.DatabaseFunction(inputSample, outputSample)

print("database=", database)

# Does it work?
x = ot.Point(dimension, 1.8)
print("x=", x)
print("database(x)=", database(x))

# DatabaseEvaluation direct coverage (no prints to keep expout stable)
dbEval = ot.DatabaseEvaluation(inputSample, outputSample)
_ = repr(dbEval)
_ = str(dbEval)
assert dbEval.getInputDimension() == dimension
assert dbEval.getOutputDimension() == 1
assert dbEval.isActualImplementation()
_ = dbEval.getCallsNumber()
_ = dbEval.getParameter()
_ = dbEval.getParameterDescription()
ott.assert_almost_equal(dbEval.getInputSample(), inputSample, 1e-12, 1e-12)
ott.assert_almost_equal(dbEval.getOutputSample(), outputSample, 1e-12, 1e-12)
_ = dbEval.getNearestNeighbourAlgorithm()
dbEval.setNearestNeighbourAlgorithm(dbEval.getNearestNeighbourAlgorithm())
ott.assert_almost_equal(dbEval(x), database(x), 1e-12, 1e-12)
ott.assert_almost_equal(dbEval(inputSample), outputSample, 1e-12, 1e-12)
ott.assert_almost_equal(dbEval(ot.Sample([[1.1, 1.1]])), dbEval(ot.Sample([[1.1, 1.1]])), 1e-12, 1e-12)
ott.assert_almost_equal(dbEval.getMarginal(0)(x), database(x), 1e-12, 1e-12)
assert dbEval == ot.DatabaseEvaluation(inputSample, outputSample)
assert not (dbEval != ot.DatabaseEvaluation(inputSample, outputSample))
otherOut = ot.Sample(0, 1)
otherOut.add([6.0])
otherOut.add([7.0])
assert dbEval != ot.DatabaseEvaluation(inputSample, otherOut)
ott.assert_almost_equal(database.getEvaluation()(x), database(x), 1e-12, 1e-12)
ott.assert_almost_equal(database.getEvaluation()(inputSample), outputSample, 1e-12, 1e-12)
_ = str(database.getEvaluation())
_ = database.getCallsNumber()
assert database == database
assert not (database != database)
dbEval.setInputSample(inputSample)
dbEval.setOutputSample(outputSample)
with ott.assert_raises(Exception):
    dbEval(ot.Point([1.0]))
with ott.assert_raises(Exception):
    dbEval(ot.Sample([[1.0]]))
with ott.assert_raises(Exception):
    ot.DatabaseEvaluation(ot.Sample(0, dimension), outputSample)
with ott.assert_raises(Exception):
    ot.DatabaseEvaluation(inputSample, ot.Sample(0, 1))
with ott.assert_raises(Exception):
    ot.DatabaseEvaluation(inputSample, ot.Sample([[1.0]]))
with ott.assert_raises(Exception):
    ot.DatabaseEvaluation().setInputSample(ot.Sample([[1.0, 2.0], [3.0, 4.0]]))
with ott.assert_raises(Exception):
    ot.DatabaseEvaluation(inputSample, outputSample).setOutputSample(ot.Sample([[1.0]]))
_ = repr(ot.DatabaseEvaluation())
