#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import copy
import os

ot.TESTPREAMBLE()

myFunc = ot.SymbolicFunction("x", "x^2")
tg = ot.RegularGrid(0.0, 0.2, 6)
mySpatialFunc = ot.ValueFunction(myFunc, tg)

print("mySpatialFunc=", mySpatialFunc)
# Get the input and output description
print("mySpatialFunc input description=", mySpatialFunc.getInputDescription())
print("mySpatialFunc output description=", mySpatialFunc.getOutputDescription())
# Get the input and output dimension, based on description
print("mySpatialFunc input dimension=", mySpatialFunc.getInputDimension())
print("mySpatialFunc output dimension=", mySpatialFunc.getOutputDimension())
# Create a TimeSeries
data = ot.Sample(tg.getN(), myFunc.getInputDimension())
for i in range(data.getSize()):
    for j in range(data.getDimension()):
        data[i, j] = i * data.getDimension() + j
ts = ot.TimeSeries(tg, data)
print("input time series=", ts)
print("output time series=", mySpatialFunc(ts))
# Get the number of calls
print("called ", mySpatialFunc.getCallsNumber(), " times")

# extra coverage (no prints: output is checked against .expout)

assert ot.ValueFunction() == ot.ValueFunction()
assert mySpatialFunc == mySpatialFunc
assert not (mySpatialFunc != mySpatialFunc)
assert len(repr(mySpatialFunc)) > 0
assert len(str(mySpatialFunc)) > 0
assert mySpatialFunc.getFunction() == myFunc
assert mySpatialFunc.isActingPointwise()
# marginals
marginal = ot.FieldFunction(mySpatialFunc.getMarginal(0))
assert marginal.getOutputDimension() == 1
ott.assert_almost_equal(marginal(ts), mySpatialFunc(ts), 1e-14, 1e-14)
marginals = ot.FieldFunction(mySpatialFunc.getMarginal([0]))
assert marginals.getOutputDimension() == 1
with ott.assert_raises(Exception):
    mySpatialFunc.getMarginal(5)
with ott.assert_raises(Exception):
    mySpatialFunc.getMarginal([0, 5])
# operator() error branches
with ott.assert_raises(Exception):
    mySpatialFunc(ot.Sample([[1.0, 2.0]] * 6))
with ott.assert_raises(Exception):
    mySpatialFunc(ot.Sample([[1.0]] * 4))
# EvaluationImplementation ctor
mySpatialFunc2 = ot.ValueFunction(myFunc.getEvaluation().getImplementation(), tg)
ott.assert_almost_equal(mySpatialFunc2(ts), mySpatialFunc(ts), 1e-14, 1e-14)
# clone

assert copy.copy(mySpatialFunc) == mySpatialFunc
# save/load
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("value_function.xml"))
study.add("f", mySpatialFunc)
study.save()
loaded_study = ot.Study()
loaded_study.setStorageManager(ot.XMLStorageManager("value_function.xml"))
loaded_study.load()
loaded = ot.FieldFunction()
loaded_study.fillObject("f", loaded)
assert loaded == mySpatialFunc

os.remove("value_function.xml")
