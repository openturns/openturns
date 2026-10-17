#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott
import os

ot.TESTPREAMBLE()

mesh = ot.RegularGrid(0.0, 0.1, 11)


class FUNC(ot.OpenTURNSPythonPointToFieldFunction):
    def __init__(self):
        super(FUNC, self).__init__(2, mesh, 2)
        self.setInputDescription(["R", "S"])
        self.setOutputDescription(["T", "U"])
        self.mesh_ = ot.RegularGrid(0.0, 0.1, 11)

    def _exec(self, X):
        size = self.mesh_.getVerticesNumber()
        values = [ot.Point(X) * i for i in range(size)]
        Y = ot.Field(self.mesh_, values)
        return Y


F = FUNC()
print(
    "in_dim=" + str(F.getInputDimension()) + " out_dim=" + str(F.getOutputDimension())
)

print((F((10.0, 5.0))))

print((F(((10.0, 5.0), (6.0, 7.0)))))


# Instance creation
myFunc = ot.PointToFieldFunction(F)

# Copy constructor
newFunc = ot.PointToFieldFunction(myFunc)

print(("myFunc input dimension= " + str(myFunc.getInputDimension())))
print(("myFunc output dimension= " + str(myFunc.getOutputDimension())))

inPt = ot.Point(2, 2.0)
print((repr(inPt)))

outPt = myFunc(inPt)
print((repr(outPt)))

outPt = myFunc((10.0, 11.0))
print((repr(outPt)))

inSample = ot.Sample(10, 2)
for i in range(10):
    inSample[i] = ot.Point((i, i))
print((repr(inSample)))

outSample = myFunc(inSample)
print((repr(outSample)))

outSample = myFunc(((100.0, 100.0), (101.0, 101.0), (102.0, 102.0)))
print((repr(outSample)))

assert ot.PointToFieldFunction(myFunc) == myFunc

with ott.assert_raises(Exception):
    myFunc([1.0, 2.0, 3.0])


class BadPointToFieldFunction(ot.OpenTURNSPythonPointToFieldFunction):
    def __init__(self):
        super().__init__(2, mesh, 2)

    def _exec(self, X):
        return [[1.0] * 5] * 11


with ott.assert_raises(Exception):
    ot.PointToFieldFunction(BadPointToFieldFunction())([1.0, 2.0])


class NonSequencePointToFieldFunction(ot.OpenTURNSPythonPointToFieldFunction):
    def __init__(self):
        super().__init__(2, mesh, 2)

    def _exec(self, X):
        return 42


with ott.assert_raises(Exception):
    ot.PointToFieldFunction(NonSequencePointToFieldFunction())([1.0, 2.0])


class MarginalPointToFieldFunction(ot.OpenTURNSPythonPointToFieldFunction):
    def __init__(self):
        mesh = ot.RegularGrid(0.0, 0.1, 11)
        super().__init__(2, mesh, 2)
        self.setInputDescription(["R", "S"])
        self.setOutputDescription(["T", "U"])

    def _exec(self, X):
        size = self.getOutputMesh().getVerticesNumber()
        return [ot.Point(X) * i for i in range(size)]


# Marginals of a point to field function, see issue #2801
marginalFunc = ot.PointToFieldFunction(MarginalPointToFieldFunction())
marginal0 = marginalFunc.getMarginal(0)
assert marginal0.getOutputDimension() == 1
assert list(marginal0.getOutputDescription()) == ["T"]
marginal1 = marginalFunc.getMarginal(1)
ott.assert_almost_equal(marginal1([1.0, 2.0]), marginalFunc([1.0, 2.0]).getMarginal(1))
with ott.assert_raises(Exception):
    marginalFunc.getMarginal(2)
# Nested marginals compose indices, count calls and persist, see issue #2801
nested = marginalFunc.getMarginal([1, 0]).getMarginal(1)
assert nested.getOutputDimension() == 1
assert list(nested.getOutputDescription()) == ["T"]
ott.assert_almost_equal(nested([1.0, 2.0]), marginal0([1.0, 2.0]))
with ott.assert_raises(Exception):
    marginal0.getMarginal(1)
before = marginal0.getCallsNumber()
marginal0([1.0, 2.0])
assert marginal0.getCallsNumber() == before + 1
assert list(marginal1([1.0, 2.0]).getDescription()) == ["U"]
# Persistence round-trip through a Study. The parent chain is C++-backed
# (a VertexValue function seen through a no-op Parametric wrapper, the
# only concrete C++ route to the base getMarginal implementation): the
# filtering wrapper must survive save/load with its state intact.
mesh2d = ot.IntervalMesher([2] * 2).build(ot.Interval([-1.0] * 2, [1.0] * 2))
cppParent = ot.PointToFieldFunction(
    ot.ParametricPointToFieldFunction(
        ot.PointToFieldFunction(
            ot.VertexValuePointToFieldFunction(
                ot.LinearFunction([0.0, 0.0], [1.0, 2.0], ot.Matrix([[1.0, 0.0], [0.0, 1.0]])),
                mesh2d,
            )
        ),
        [],
        [],
    )
)
cppMarginal = cppParent.getMarginal(1)
assert cppMarginal.getOutputDimension() == 1
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("marginal_ptf.xml"))
study.add("marginal", cppMarginal)
study.save()
loadedStudy = ot.Study("marginal_ptf.xml")
loadedStudy.load()
loaded = ot.PointToFieldFunction()
loadedStudy.fillObject("marginal", loaded)
assert loaded.getOutputDimension() == 1
assert list(loaded.getOutputDescription()) == list(cppMarginal.getOutputDescription())
os.remove("marginal_ptf.xml")
