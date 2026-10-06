#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

function1 = ot.SymbolicFunction(["x0", "x1", "x2"], ["x0^2+2*x1+3*x2^3"])
function2 = ot.SymbolicFunction(
    ["x0", "x1", "x2"], ["cos(x0*sin(x2+x1))", "exp(x1 - x0 * sin(x2))"]
)
evaluation = ot.AggregatedEvaluation([function1, function2])
print("evaluation=", evaluation)
point = [4.0, -4.0, 1.0]
print("function 1 at", point, "=", function1(point))
print("function 2 at", point, "=", function2(point))
print("evaluation at", point, "=", evaluation(point))

# Equality tests
e1 = ot.AggregatedEvaluation([function1])
e2 = ot.AggregatedEvaluation([function1])
assert e1 == e2, "same functions should be equal"
assert e1 != evaluation, "different number of functions"
e3 = ot.AggregatedEvaluation([function2])
assert e1 != e3, "different functions"
e4 = ot.AggregatedEvaluation([function1, function2])
assert evaluation == e4, "same pair of functions"

# AggregatedEvaluation extra coverage (no prints to keep expout stable)
_ = repr(evaluation)
_ = str(evaluation)
assert evaluation.getInputDimension() == 3
assert evaluation.getOutputDimension() == 3
assert evaluation.isActualImplementation()
assert not evaluation.isLinear()
_ = evaluation.isLinearlyDependent(0)
_ = evaluation.getCallsNumber()
_ = evaluation.getParameter()
_ = evaluation.getParameterDescription()
_ = evaluation.parameterGradient(point)
_ = evaluation.getFunctionsCollection()
ott.assert_almost_equal(evaluation(point), [function1(point)[0], function2(point)[0], function2(point)[1]], 1e-12, 1e-12)
ott.assert_almost_equal(evaluation(ot.Sample([point, point]))[0], evaluation(point), 1e-12, 1e-12)
ott.assert_almost_equal(evaluation.getMarginal(0)(point), [evaluation(point)[0]], 1e-12, 1e-12)
ott.assert_almost_equal(evaluation.getMarginal([0, 2])(point), [evaluation(point)[0], evaluation(point)[2]], 1e-12, 1e-12)
ott.assert_almost_equal(evaluation.getMarginal([2, 0])(point), [evaluation(point)[2], evaluation(point)[0]], 1e-12, 1e-12)
_ = evaluation.getMarginal([])
evaluation.setParameter(evaluation.getParameter())
with ott.assert_raises(Exception):
    evaluation.setParameter([0.0])
with ott.assert_raises(Exception):
    evaluation(ot.Point([1.0]))
with ott.assert_raises(Exception):
    evaluation(ot.Sample([[1.0]]))
with ott.assert_raises(Exception):
    evaluation.getMarginal(10)
with ott.assert_raises(Exception):
    evaluation.getMarginal([0, 10])
with ott.assert_raises(Exception):
    ot.AggregatedEvaluation([])
with ott.assert_raises(Exception):
    ot.AggregatedEvaluation([function1, ot.SymbolicFunction(["a", "b"], ["a"])])
with ott.assert_raises(Exception):
    ot.AggregatedEvaluation([function1, ot.Function(ot.NoEvaluation())])
emptyAgg = ot.AggregatedEvaluation()
_ = repr(emptyAgg)
_ = str(emptyAgg)
with ott.assert_raises(Exception):
    emptyAgg.getInputDimension()
with ott.assert_raises(Exception):
    emptyAgg.setFunctionsCollection([])
resetAgg = ot.AggregatedEvaluation([function1])
resetAgg.setFunctionsCollection([function1, function2])
assert resetAgg == evaluation
assert not (resetAgg != evaluation)
