#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

# Create a KarhunenLoeveResult
mesh = ot.IntervalMesher([9]).build(ot.Interval(-1.0, 1.0))
cov1D = ot.AbsoluteExponential([1.0])
algo = ot.KarhunenLoeveP1Algorithm(mesh, cov1D, 0.0)
algo.run()
result = algo.getResult()
lifting = ot.KarhunenLoeveLifting(result)
# Construction based on a Function followed by a PointToFieldFunction
dimension = lifting.getInputDimension()
function = ot.SymbolicFunction(
    ot.Description.BuildDefault(dimension, "x"),
    ot.Description.BuildDefault(dimension, "x"),
)
# Create an instance
myFunc = ot.PointToFieldConnection(lifting, function)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

# Construction based on a PointToFieldFunction followed by a
# FieldToPointFunction
fieldFunction = ot.ValueFunction(ot.SymbolicFunction("x", "x"), mesh)
# Create an instance
myFunc = ot.PointToFieldConnection(fieldFunction, lifting)

print("myFunc=", myFunc)
# Get the input and output description
print("myFunc input description=", myFunc.getInputDescription())
print("myFunc output description=", myFunc.getOutputDescription())
# Get the input and output dimension
print("myFunc input dimension=", myFunc.getInputDimension())
print("myFunc output dimension=", myFunc.getOutputDimension())
# Connection on a point
point = [1.0] * myFunc.getInputDimension()
print("point=", point)
print("myFunc(point)=", myFunc(point))
# Get the number of calls
print("called ", myFunc.getCallsNumber(), " times")

# Extra coverage (no output to preserve expout)
assert ot.PointToFieldConnection() == ot.PointToFieldConnection()
assert myFunc == myFunc
assert len(repr(myFunc)) > 0
assert len(str(myFunc)) > 0
# accessors
assert myFunc.getFieldFunction().getOutputDimension() == myFunc.getOutputDimension()
assert (
    myFunc.getPointToFieldFunction().getInputDimension()
    == myFunc.getInputDimension()
)
# evaluation on a sample (ProcessSample path)
inDim = myFunc.getInputDimension()
sample_in = ot.Sample(3, inDim)
for i in range(3):
    for j in range(inDim):
        sample_in[i, j] = 0.5 * (i + 1)
result_sample = myFunc(sample_in)
assert result_sample.getSize() == 3
# wrong dimensions
with ott.assert_raises(Exception):
    myFunc(ot.Point([1.0]))
with ott.assert_raises(Exception):
    myFunc(ot.Sample(3, inDim + 1))
# marginal error paths
with ott.assert_raises(Exception):
    myFunc.getMarginal(myFunc.getOutputDimension())
with ott.assert_raises(Exception):
    myFunc.getMarginal(ot.Indices([0]))
# other composition order accessors
dimension = lifting.getInputDimension()
function2 = ot.SymbolicFunction(
    ot.Description.BuildDefault(dimension, "x"),
    ot.Description.BuildDefault(dimension, "x"),
)
first = ot.PointToFieldConnection(lifting, function2)
assert first == first
assert len(first.getFunction().getInputDescription()) == dimension
assert first(first.getInputDimension() * [0.25]).getSize() == mesh.getVerticesNumber()
assert first(ot.Sample(2, first.getInputDimension())).getSize() == 2
with ott.assert_raises(Exception):
    first.getMarginal(ot.Indices([0]))
# save / load roundtrip
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("ptfc_test.xml"))
study.add("myFunc", myFunc)
study.save()
study2 = ot.Study()
study2.setStorageManager(ot.XMLStorageManager("ptfc_test.xml"))
study2.load()
loaded = ot.PointToFieldConnection()
study2.fillObject("myFunc", loaded)
assert loaded == myFunc

os.remove("ptfc_test.xml")
