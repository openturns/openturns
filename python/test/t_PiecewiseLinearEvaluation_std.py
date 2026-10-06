#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ref = ot.SymbolicFunction("x", "sin(x)")
size = 12
locations = ot.Point(size)
values = ot.Point(size)
# Build locations/values with non-increasing locations
for i in range(size):
    locations[i] = 10.0 * i * i / (size - 1.0) / (size - 1.0)
    values[i] = ref([locations[i]])[0]

evaluation = ot.PiecewiseLinearEvaluation(locations, values)
print("evaluation=", evaluation)
# Check the values
for i in range(2 * size):
    x = [-1.0 + 12.0 * i / (2.0 * size - 1.0)]
    print("f( %.12g )=" % x[0], evaluation(x), ", ref=", ref(x))

# Test exception enableExtrapolation
locations = [1.0, 2.0, 3.0, 4.0, 5.0]
values = [-2.0, 2.0, 1.0, 3.0, 5.0]
evaluation = ot.PiecewiseLinearEvaluation(locations, values)
evaluation.setEnableExtrapolation(False)
f = ot.Function(evaluation)
with ott.assert_raises(TypeError):
    f([-12.5])

# Alternate ctor with Sample values, accessors, comparison, setters
locations2 = [0.0, 1.0, 2.0]
values2 = [[0.0, 1.0], [1.0, 3.0], [4.0, 9.0]]
evaluation2 = ot.PiecewiseLinearEvaluation(locations2, values2)
assert evaluation2.getInputDimension() == 1
assert evaluation2.getOutputDimension() == 2
assert list(evaluation2.getLocations()) == locations2
assert evaluation2.getEnableExtrapolation()
ott.assert_almost_equal(evaluation2([0.5]), [0.5, 2.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    evaluation2([[0.5], [1.5]]), [[0.5, 2.0], [2.5, 6.0]], 1e-14, 1e-14
)
evaluation2.setEnableExtrapolation(True)
ott.assert_almost_equal(evaluation2([-1.0]), [0.0, 1.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    evaluation2([[10.0]]), [[4.0, 9.0]], 1e-14, 1e-14
)
evaluation2.setEnableExtrapolation(False)
assert not evaluation2.getEnableExtrapolation()
_ = repr(evaluation2)
_ = str(evaluation2)
assert evaluation2 == evaluation2
assert evaluation2 != evaluation
_ = evaluation2.getCallsNumber()
with ott.assert_raises(Exception):
    evaluation2([1.0, 2.0])
with ott.assert_raises(Exception):
    evaluation2([[1.0, 2.0]])
with ott.assert_raises(Exception):
    evaluation2([-1.0])
with ott.assert_raises(Exception):
    evaluation2([5.0])
with ott.assert_raises(Exception):
    evaluation2([[5.0]])
with ott.assert_raises(Exception):
    evaluation2([[-1.0]])
with ott.assert_raises(Exception):
    ot.PiecewiseLinearEvaluation([0.0], [1.0, 2.0])
with ott.assert_raises(Exception):
    ot.PiecewiseLinearEvaluation([0.0, 0.0], [1.0, 1.0])
with ott.assert_raises(Exception):
    evaluation2.setLocations([0.0, 1.0])
with ott.assert_raises(Exception):
    evaluation2.setLocations([0.0, 0.0, 1.0])
with ott.assert_raises(Exception):
    evaluation2.setValues([1.0, 2.0])
with ott.assert_raises(Exception):
    evaluation2.setValues([[1.0], [2.0]])
with ott.assert_raises(Exception):
    evaluation2.setLocationsAndValues([0.0], [[1.0], [2.0]])
with ott.assert_raises(Exception):
    evaluation2.setLocationsAndValues([], [[]])
# setLocations reorders the values along the sorted locations
evaluation2.setLocations([2.0, 0.0, 1.0])
assert list(evaluation2.getLocations()) == [0.0, 1.0, 2.0]
ott.assert_almost_equal(
    evaluation2.getValues(),
    [[1.0, 3.0], [4.0, 9.0], [0.0, 1.0]],
    1e-14,
    1e-14,
)
evaluation2.setValues([[1.0, 3.0], [4.0, 9.0], [0.0, 1.0]])
evaluation2.setValues([1.0, 2.0, 3.0])
assert evaluation2.getOutputDimension() == 1
evaluation2.setLocationsAndValues([0.0, 1.0], [[0.0], [1.0]])
assert list(evaluation2.getLocations()) == [0.0, 1.0]
ott.assert_almost_equal(evaluation2([0.5]), [0.5], 1e-14, 1e-14)
# Single-location shortcut returns the stored value everywhere
singleEval = ot.PiecewiseLinearEvaluation([1.0], [2.0])
ott.assert_almost_equal(singleEval([0.0]), [2.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    singleEval([[0.0], [5.0]]), [[2.0], [2.0]], 1e-14, 1e-14
)
