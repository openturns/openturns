#! /usr/bin/env python

import openturns as ot
import math as m
import openturns.testing as ott
import os

ot.TESTPREAMBLE()


inputVars = ot.Description(["t"])
formula = ot.Description(["sin(t)", "cos(t)"])
myFunc = ot.SymbolicFunction(inputVars, formula)
tg = ot.RegularGrid(0.0, 0.1, 11)
myInverseTrendFunc = ot.InverseTrendTransform(myFunc, tg)

print("myInverseTrendFunc=", myInverseTrendFunc)
# Get the input description and dimension
print("myInverseTrendFunc input description=", myInverseTrendFunc.getInputDescription())
print("myInverseTrendFunc input dimension=", myInverseTrendFunc.getInputDimension())
# Get the output description and dimension
print(
    "myInverseTrendFunc output description=", myInverseTrendFunc.getOutputDescription()
)
print("myInverseTrendFunc output dimension=", myInverseTrendFunc.getOutputDimension())
# Create a TimeSeries
data = ot.Sample(tg.getN(), formula.getSize())
for i in range(data.getSize()):
    t = tg.getStart() + i * tg.getStep()
    data[i, 0] = i + 1 + m.sin(t)
    data[i, 1] = i + m.cos(t)
ts = ot.TimeSeries(tg, data)
print("input time series =  ")
print(ts)
print("output time series = ")
print(myInverseTrendFunc(ts))
# Get the number of calls
print("called ", myInverseTrendFunc.getCallsNumber(), " times")

# Extra coverage: default ctor, other ctors, accessors, save/load (no prints to keep expout stable)

defaultTransform = ot.InverseTrendTransform()
assert isinstance(repr(defaultTransform), str)
assert isinstance(str(defaultTransform), str)
with ott.assert_raises(Exception):
    defaultTransform.getTrendFunction()
# constructors from Evaluation and EvaluationImplementation
fromEval = ot.InverseTrendTransform(myFunc.getEvaluation(), tg)
ott.assert_almost_equal(fromEval.getTrendFunction()([0.0]), myFunc([0.0]), 1e-12, 1e-12)
fromImpl = ot.InverseTrendTransform(myFunc.getEvaluation().getImplementation(), tg)
ott.assert_almost_equal(fromImpl.getTrendFunction()([0.0]), myFunc([0.0]), 1e-12, 1e-12)
# clone, repr/str, inverse accessors
cloned = ot.InverseTrendTransform(myInverseTrendFunc)
assert isinstance(repr(myInverseTrendFunc), str)
assert isinstance(str(myInverseTrendFunc), str)
ott.assert_almost_equal(
    cloned.getTrendFunction()([0.0]), myFunc([0.0]), 1e-12, 1e-12
)
inverse = myInverseTrendFunc.getInverse()
assert inverse.getInputDimension() == myInverseTrendFunc.getInputDimension()
ott.assert_almost_equal(myInverseTrendFunc.getTrendFunction()([0.5]), myFunc([0.5]), 1e-12, 1e-12)
# save/load round-trip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("testInverseTrendTransform.xml"))
study.add("transform", myInverseTrendFunc)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("testInverseTrendTransform.xml"))
loadedStudy.load()
assert loadedStudy.hasObject("transform")

os.remove("testInverseTrendTransform.xml")
