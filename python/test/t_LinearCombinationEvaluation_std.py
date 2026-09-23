#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()


# First, build two functions from R^3->R
inVar = ot.Description(3)
inVar[0] = "x1"
inVar[1] = "x2"
inVar[2] = "x3"
outVar = ot.Description(1)
outVar[0] = "y"
formula = ot.Description(1)
formula[0] = "x1^3 * sin(x2 + 2.5 * x3) - (x1 + x2)^2 / (1.0 + x3^2)"
functions = []
functions.append(ot.SymbolicFunction(inVar, formula))
formula[0] = "exp(-x1 * x2 + x3) / cos(1.0 + x2 * x3 - x1)"
functions.append(ot.SymbolicFunction(inVar, formula))
# Second, build the weights
coefficients = ot.Point(2)
coefficients[0] = 0.3
coefficients[1] = 2.9
myEvaluation = ot.LinearCombinationEvaluation(functions, coefficients)
inPoint = ot.Point(3)
inPoint[0] = 1.2
inPoint[1] = 2.3
inPoint[2] = 3.4
print("myEvaluation=", myEvaluation)
print("Value at ", inPoint, "=", myEvaluation(inPoint))

# LinearCombinationEvaluation extra coverage (no prints)
_ = repr(myEvaluation)
_ = str(myEvaluation)
assert myEvaluation.getInputDimension() == 3
assert myEvaluation.getOutputDimension() == 1
ott.assert_almost_equal(myEvaluation(ot.Sample([inPoint, inPoint]))[0], myEvaluation(inPoint), 1e-14, 1e-14)
_ = myEvaluation.getCoefficients()
_ = myEvaluation.getFunctionsCollection()
_ = myEvaluation.getParameter()
_ = myEvaluation.getParameterDescription()
myEvaluation.setParameter(myEvaluation.getParameter())
_ = myEvaluation.parameterGradient(inPoint)
_ = myEvaluation.getMarginal(0)
_ = myEvaluation.getMarginal([0])
assert not myEvaluation.isLinear()
_ = myEvaluation.isLinearlyDependent(0)
with ott.assert_raises(Exception):
    myEvaluation([1.0])
with ott.assert_raises(Exception):
    myEvaluation.getMarginal(5)
with ott.assert_raises(Exception):
    ot.LinearCombinationEvaluation([], [])
with ott.assert_raises(Exception):
    ot.LinearCombinationEvaluation(functions, [0.3])
with ott.assert_raises(Exception):
    ot.LinearCombinationEvaluation([functions[0], ot.SymbolicFunction(["x"], ["x"])], [0.5, 0.5])
with ott.assert_raises(Exception):
    ot.LinearCombinationEvaluation([functions[0], ot.SymbolicFunction(["x1", "x2", "x3"], ["x1", "x2"])], [0.5, 0.5])
with ott.assert_raises(Exception):
    myEvaluation.setParameter([0.0])
# small-coefficient filtering + isZero branch
tinyEval = ot.LinearCombinationEvaluation(functions, [1e-15, 0.0])
ott.assert_almost_equal(tinyEval(inPoint), [0.0], 1e-14, 1e-14)
ott.assert_almost_equal(tinyEval(ot.Sample([inPoint])), [[0.0]], 1e-14, 1e-14)
_ = str(tinyEval)
with ott.assert_raises(Exception):
    tinyEval.isLinearlyDependent(5)
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("lincomb_eval.xml"))
study.add("ev", ot.Function(myEvaluation))
study.save()
