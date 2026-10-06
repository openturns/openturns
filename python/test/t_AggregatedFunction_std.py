#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

# First, build two functions from R^3->R^2
functions = list()
functions.append(
    ot.SymbolicFunction(
        ["x1", "x2", "x3"],
        [
            "x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)",
            "x1^1 * sin(x3 + 2.5 * x1) - (x2 + x3)^2 / (1.0 + x1^2)",
        ],
    )
)
functions.append(
    ot.SymbolicFunction(
        ["x1", "x2", "x3"],
        [
            "exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)",
            "exp(-x2 * x3 + x1) / cos(1.0 + x3 * x1 - x2)",
        ],
    )
)
# Second, build the function
myFunction = ot.AggregatedFunction(functions)
inPoint = ot.Point([1.2, 2.3, 3.4])
print("myFunction=", myFunction)
print("Value at ", inPoint, "=", myFunction(inPoint))
print("Gradient at ", inPoint, "=", myFunction.gradient(inPoint))
ot.PlatformInfo.SetNumericalPrecision(5)
print("Hessian at ", inPoint, "=", myFunction.hessian(inPoint))
for i in range(myFunction.getOutputDimension()):
    print("Marginal ", i, "=", myFunction.getMarginal(i))
print("Marginal (0,1)=", myFunction.getMarginal([0, 1]))
print("Marginal (0,2)=", myFunction.getMarginal([0, 2]))
print("Marginal (1,2)=", myFunction.getMarginal([1, 2]))

# Equality tests
f1 = ot.AggregatedFunction([functions[0]])
f2 = ot.AggregatedFunction([functions[0]])
assert f1 == f2, "same functions should be equal"
assert f1 != myFunction, "different number of functions"
f3 = ot.AggregatedFunction([functions[1]])
assert f1 != f3, "different functions"
f4 = ot.AggregatedFunction(functions)
assert myFunction == f4, "same pair of functions"

# AggregatedEvaluation/Gradient/Hessian indirect coverage (no prints)
aggGrad = myFunction.getGradient()
_ = repr(aggGrad)
_ = str(aggGrad)
assert aggGrad.getInputDimension() == myFunction.getInputDimension()
assert aggGrad.getOutputDimension() == myFunction.getOutputDimension()
_ = aggGrad.getCallsNumber()
_ = aggGrad.getParameter()
ott.assert_almost_equal(aggGrad.gradient(inPoint), myFunction.gradient(inPoint), 1e-12, 1e-12)
assert aggGrad == myFunction.getGradient()
assert not (aggGrad != myFunction.getGradient())
with ott.assert_raises(Exception):
    aggGrad.gradient(ot.Point([1.0]))
aggHess = myFunction.getHessian()
_ = repr(aggHess)
_ = str(aggHess)
assert aggHess.getInputDimension() == myFunction.getInputDimension()
assert aggHess.getOutputDimension() == myFunction.getOutputDimension()
_ = aggHess.getCallsNumber()
_ = aggHess.getParameter()
ott.assert_almost_equal(aggHess.hessian(inPoint), myFunction.hessian(inPoint), 1e-12, 1e-12)
assert aggHess == myFunction.getHessian()
assert not (aggHess != myFunction.getHessian())
with ott.assert_raises(Exception):
    aggHess.hessian(ot.Point([1.0]))
ott.assert_almost_equal(myFunction(ot.Sample([inPoint, inPoint]))[0], myFunction(inPoint), 1e-12, 1e-12)
ott.assert_almost_equal(myFunction.getEvaluation()(inPoint), myFunction(inPoint), 1e-12, 1e-12)
ott.assert_almost_equal(myFunction.getEvaluation()(ot.Sample([inPoint])), myFunction(ot.Sample([inPoint])), 1e-12, 1e-12)
_ = str(myFunction.getEvaluation())
_ = myFunction.getEvaluation().getCallsNumber()
_ = myFunction.getCallsNumber()
ott.assert_almost_equal(myFunction.getMarginal([2, 0])(inPoint), [myFunction(inPoint)[2], myFunction(inPoint)[0]], 1e-12, 1e-12)
with ott.assert_raises(Exception):
    myFunction(ot.Point([1.0]))
with ott.assert_raises(Exception):
    myFunction.getEvaluation()(ot.Point([1.0]))
with ott.assert_raises(Exception):
    myFunction.getMarginal(10)
with ott.assert_raises(Exception):
    myFunction.getMarginal([0, 10])
with ott.assert_raises(Exception):
    ot.AggregatedFunction([])
with ott.assert_raises(Exception):
    ot.AggregatedFunction([functions[0], ot.SymbolicFunction(["a", "b"], ["a"])])
