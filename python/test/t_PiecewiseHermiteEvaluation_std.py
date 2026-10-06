#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ref = ot.SymbolicFunction("x", "sin(x)")
size = 12
locations = [0.0] * size
values = [0.0] * size
derivatives = [0.0] * size
# Build locations/values/derivatives with non-increasing locations
for i in range(size):
    locations[i] = 10.0 * i * i / (size - 1.0) / (size - 1.0)
    values[i] = ref([locations[i]])[0]
    derivatives[i] = ref.gradient([locations[i]])[0, 0]

evaluation = ot.PiecewiseHermiteEvaluation(locations, values, derivatives)
print("evaluation=", evaluation)
# Check the values
X = [[-1.0 + 12.0 * i / (2.0 * size - 1.0)] for i in range(2 * size)]
for x in X:
    print("f( %.12g )=" % x[0], evaluation(x), ", ref=", ref(x))
Y = evaluation(X)
print(Y)

# Test exception enableExtrapolation
locations = [1.0, 2.0, 3.0, 4.0, 5.0]
values = [-2.0, 2.0, 1.0, 3.0, 5.0]
derivatives = [0.0] * 5
evaluation = ot.PiecewiseHermiteEvaluation(locations, values, derivatives)
evaluation.setEnableExtrapolation(False)
f = ot.Function(evaluation)
with ott.assert_raises(TypeError):
    f([-12.5])

# Test derivative: compare with exact derivative of sin(x)
# Build regular Hermite interpolation of sin(x) and test derivative = cos(x)
ref_der = ot.SymbolicFunction("x", "cos(x)")
size_der = 20
locations_der = [10.0 * i / (size_der - 1.0) for i in range(size_der)]
values_der = [ref([x])[0] for x in locations_der]
derivatives_der = [ref_der([x])[0] for x in locations_der]
evaluation_der = ot.PiecewiseHermiteEvaluation(locations_der, values_der, derivatives_der)
# Interior derivative
for x in [0.5, 2.5, 5.0, 7.5, 9.5]:
    d = evaluation_der.derivate([x])[0]
    d_ref = ref_der([x])[0]
    ott.assert_almost_equal(d, d_ref, 5e-2, 0.0)

# Extrapolation left: must return derivatives[0] not values[0]
evaluation_der.setEnableExtrapolation(True)
d_left = evaluation_der.derivate([-1.0])[0]
d_left_ref = derivatives_der[0]
ott.assert_almost_equal(d_left, d_left_ref)

# Extrapolation right: must return derivatives[-1] not values[-1]
d_right = evaluation_der.derivate([12.0])[0]
d_right_ref = derivatives_der[-1]
ott.assert_almost_equal(d_right, d_right_ref)

# Multi-dimensional derivative test with cubic functions
# f(x)=[x^2, x^3], f'(x)=[2x, 3x^2]
loc_2d = [0.0, 0.5, 1.0, 2.0]
val_2d = [[x**2, x**3] for x in loc_2d]
der_2d = [[2 * x, 3 * x**2] for x in loc_2d]
eval_2d = ot.PiecewiseHermiteEvaluation(loc_2d, val_2d, der_2d)
for x in [0.0, 0.25, 0.5, 0.75, 1.0, 1.5, 2.0]:
    d = eval_2d.derivate([x])
    d_ref = [2 * x, 3 * x**2]
    ott.assert_almost_equal(d, d_ref, 1e-14, 0.0)

# Alternate ctor with Sample values, accessors, comparison, setters
locationsH = [0.0, 1.0, 2.0]
valuesH = [[0.0], [1.0], [8.0]]
derivativesH = [[0.0], [2.0], [8.0]]
evaluationH = ot.PiecewiseHermiteEvaluation(
    locationsH, valuesH, derivativesH
)
assert evaluationH.getInputDimension() == 1
assert evaluationH.getOutputDimension() == 1
assert list(evaluationH.getLocations()) == locationsH
ott.assert_almost_equal(evaluationH.getValues(), valuesH, 1e-14, 1e-14)
ott.assert_almost_equal(
    evaluationH.getDerivatives(), derivativesH, 1e-14, 1e-14
)
assert evaluationH.getEnableExtrapolation()
ott.assert_almost_equal(evaluationH([0.5]), [0.25], 1e-14, 1e-14)
ott.assert_almost_equal(
    evaluationH([[0.5], [1.5]]), [[0.25], [3.75]], 1e-14, 1e-14
)
ott.assert_almost_equal(evaluationH.derivate([0.5]), [1.0], 1e-12, 1e-12)
evaluationH.setEnableExtrapolation(True)
ott.assert_almost_equal(evaluationH([-1.0]), [0.0], 1e-14, 1e-14)
ott.assert_almost_equal(evaluationH.derivate([-1.0]), [0.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    evaluationH([[10.0]]), [[8.0]], 1e-14, 1e-14
)
evaluationH.setEnableExtrapolation(False)
assert not evaluationH.getEnableExtrapolation()
_ = repr(evaluationH)
_ = str(evaluationH)
assert evaluationH == evaluationH
assert evaluationH != evaluation_der
_ = evaluationH.getCallsNumber()
with ott.assert_raises(Exception):
    evaluationH([1.0, 2.0])
with ott.assert_raises(Exception):
    evaluationH([[1.0, 2.0]])
with ott.assert_raises(Exception):
    evaluationH.derivate([1.0, 2.0])
with ott.assert_raises(Exception):
    evaluationH([-1.0])
with ott.assert_raises(Exception):
    evaluationH([5.0])
with ott.assert_raises(Exception):
    evaluationH([[5.0]])
with ott.assert_raises(Exception):
    evaluationH([[-1.0]])
with ott.assert_raises(Exception):
    evaluationH.derivate([-1.0])
with ott.assert_raises(Exception):
    evaluationH.derivate([5.0])
with ott.assert_raises(Exception):
    ot.PiecewiseHermiteEvaluation([0.0], [1.0, 2.0], [1.0])
with ott.assert_raises(Exception):
    ot.PiecewiseHermiteEvaluation([0.0, 0.0], [1.0, 1.0], [0.0, 0.0])
with ott.assert_raises(Exception):
    ot.PiecewiseHermiteEvaluation(
        [0.0, 1.0], [[1.0], [2.0]], [[1.0], [2.0], [3.0]]
    )
with ott.assert_raises(Exception):
    evaluationH.setDerivatives([])
with ott.assert_raises(Exception):
    evaluationH.setDerivatives([[1.0], [2.0]])
with ott.assert_raises(Exception):
    evaluationH.setLocations([0.0])
with ott.assert_raises(Exception):
    evaluationH.setLocations([0.0, 0.0, 1.0])
with ott.assert_raises(Exception):
    evaluationH.setValues([[1.0]])
with ott.assert_raises(Exception):
    evaluationH.setLocationsValuesAndDerivatives([], [[]], [[]])
with ott.assert_raises(Exception):
    evaluationH.setLocationsValuesAndDerivatives(
        [0.0, 1.0], [[1.0], [2.0]], [[1.0, 2.0], [3.0, 4.0]]
    )
# setLocations reorders values and derivatives along sorted locations
evaluationH.setLocations([2.0, 0.0, 1.0])
assert list(evaluationH.getLocations()) == [0.0, 1.0, 2.0]
ott.assert_almost_equal(
    evaluationH.getValues(), [[1.0], [8.0], [0.0]], 1e-14, 1e-14
)
ott.assert_almost_equal(
    evaluationH.getDerivatives(), [[2.0], [8.0], [0.0]], 1e-14, 1e-14
)
evaluationH.setValues([[1.0], [8.0], [0.0]])
evaluationH.setDerivatives([[2.0], [8.0], [0.0]])
evaluationH.setLocationsValuesAndDerivatives(
    [0.0, 1.0], [[1.0], [2.0]], [[1.0], [1.0]]
)
assert list(evaluationH.getLocations()) == [0.0, 1.0]
ott.assert_almost_equal(evaluationH([0.5]), [1.5], 1e-14, 1e-14)
# Single-location shortcut returns the stored value everywhere
singleH = ot.PiecewiseHermiteEvaluation([1.0], [2.0], [3.0])
ott.assert_almost_equal(singleH([5.0]), [2.0], 1e-14, 1e-14)
ott.assert_almost_equal(
    singleH([[0.0], [5.0]]), [[2.0], [2.0]], 1e-14, 1e-14
)
ott.assert_almost_equal(singleH.derivate([0.5]), [0.0], 1e-14, 1e-14)
