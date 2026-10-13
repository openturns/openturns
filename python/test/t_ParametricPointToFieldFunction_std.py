#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

mesh = ot.RegularGrid(0.0, 0.1, 11)


def f(X):
    size = 11
    Y = [ot.Point(X) * i for i in range(size)]
    return Y


inputDim = 2
outputDim = 2
function = ot.PythonPointToFieldFunction(inputDim, mesh, outputDim, f)
function.setInputDescription(["u1", "u2"])
function.setOutputDescription(["v1", "v2"])

# freeze u1=5.0
parametric = ot.ParametricPointToFieldFunction(function, [0], [5.0])

# properties
print("dim=", parametric.getInputDimension(), parametric.getOutputDimension())
print(
    "description=", parametric.getInputDescription(), parametric.getOutputDescription()
)
print("mesh=", parametric.getOutputMesh())

# op(Point)
x = [1.0]
y = parametric(x)
print("y=", y)
print("calls=", parametric.getCallsNumber())


# op(Sample)
X = [[1.0], [7.0]]
Y = parametric(X)
print("Y=", Y)
print("calls=", parametric.getCallsNumber())

# Extended deterministic coverage (no prints)
assert parametric.getParametersPositions() == [0]
assert parametric.getInputPositions() == [1]
assert parametric.getFunction() == function
assert parametric.getParameter() == [5.0]
parametric.setParameter([6.0])
assert parametric.getParameter() == [6.0]
assert "PointToFieldFunction" in repr(parametric)
assert "PointToFieldFunction" in str(parametric)
defaultParametric = ot.ParametricPointToFieldFunction()
assert defaultParametric.getInputDimension() == 0
with ott.assert_raises(Exception):
    ot.ParametricPointToFieldFunction(function, [0, 1, 2], [1.0, 2.0, 3.0])
with ott.assert_raises(Exception):
    ot.ParametricPointToFieldFunction(function, [0, 0], [1.0, 2.0])
with ott.assert_raises(Exception):
    ot.ParametricPointToFieldFunction(function, [5], [1.0])
with ott.assert_raises(Exception):
    ot.ParametricPointToFieldFunction(function, [0], [1.0, 2.0])
with ott.assert_raises(Exception):
    parametric([1.0, 2.0])
with ott.assert_raises(Exception):
    parametric([[1.0, 2.0], [3.0, 4.0]])

study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("parametric_point_to_field.xml"))
study.add("p", parametric)
study.save()
loadedStudy = ot.Study()
loadedStudy.setStorageManager(ot.XMLStorageManager("parametric_point_to_field.xml"))
loadedStudy.load()
loaded = ot.PointToFieldFunction()
loadedStudy.fillObject("p", loaded)
assert loaded.getInputDimension() == parametric.getInputDimension()
assert loaded.getOutputDimension() == parametric.getOutputDimension()
os.remove("parametric_point_to_field.xml")
