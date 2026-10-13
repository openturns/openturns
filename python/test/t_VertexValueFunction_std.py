#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()


myFunc = ot.SymbolicFunction(["t", "x"], ["x + t^2"])
tg = ot.RegularGrid(0.0, 0.2, 6)
myTemporalFunc = ot.VertexValueFunction(myFunc, tg)

print("myTemporalFunc=", myTemporalFunc)
# Get the input and output description
print("myTemporalFunc input description=", myTemporalFunc.getInputDescription())
print("myTemporalFunc output description=", myTemporalFunc.getOutputDescription())
# Get the input and output dimension, based on description
print("myTemporalFunc input dimension=", myTemporalFunc.getInputDimension())
print("myTemporalFunc output dimension=", myTemporalFunc.getOutputDimension())
# Create a TimeSeries
data = ot.Sample(tg.getN(), myFunc.getInputDimension() - 1)
for i in range(data.getSize()):
    for j in range(data.getDimension()):
        data[i, j] = i * data.getDimension() + j
ts = ot.TimeSeries(tg, data)
print("input time series=", ts)
print("output time series=", myTemporalFunc(ts))
# Get the number of calls
print("called ", myTemporalFunc.getCallsNumber(), " times")

# Extended deterministic coverage (no prints)
assert myTemporalFunc.isActingPointwise()
assert myTemporalFunc.getFunction() == myFunc
assert "VertexValueFunction" in repr(myTemporalFunc)
assert "x + t^2" in str(myTemporalFunc)
assert myTemporalFunc == ot.VertexValueFunction(myFunc, tg)
defaultFunc = ot.VertexValueFunction()
assert defaultFunc.getInputDimension() == 0
evalFunc = ot.VertexValueFunction(myFunc.getEvaluation(), tg)
assert evalFunc.getInputDimension() == myTemporalFunc.getInputDimension()
assert evalFunc.getOutputDimension() == myTemporalFunc.getOutputDimension()
marginal0 = ot.FieldFunction(myTemporalFunc.getMarginal(0))
assert marginal0.getOutputDimension() == 1
marginalIdx = ot.FieldFunction(myTemporalFunc.getMarginal(ot.Indices([0])))
assert marginalIdx.getOutputDimension() == 1
with ott.assert_raises(Exception):
    myTemporalFunc.getMarginal(5)
with ott.assert_raises(Exception):
    myTemporalFunc.getMarginal(ot.Indices([0, 5]))
with ott.assert_raises(Exception):
    myTemporalFunc(ot.Sample(3, 2))
with ott.assert_raises(Exception):
    ot.VertexValueFunction(
        ot.SymbolicFunction(["x"], ["x"]),
        ot.Mesh(ot.Sample([[0.0], [1.0], [2.0]]), ot.Indices([[0], [1]])),
    )

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("vertex_value_function.xml"))
typedFunc = ot.FieldFunction(myTemporalFunc)
study.add("f", typedFunc)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("vertex_value_function.xml"))
loadedStudy.load()
loaded = ot.FieldFunction()
loadedStudy.fillObject("f", loaded)
assert loaded == typedFunc
os.remove("vertex_value_function.xml")
