#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

inputDimension = 2
constant = [1, 2, 3]

f = ot.ConstantFunction(inputDimension, constant)

print(f)
print(repr(f))

assert f.getInputDimension() == inputDimension
assert f.getOutputDimension() == len(constant)
assert f == f

x = [5, 6]
y = f(x)
print(y)
ott.assert_almost_equal(y, constant)

dx = f.gradient(x)
ott.assert_almost_equal(dx, ot.Matrix(inputDimension, len(constant)))

dx2 = f.hessian(x)
ott.assert_almost_equal(dx2, ot.SymmetricTensor(inputDimension, len(constant)))

f02 = f.getMarginal([0, 2])
print(f02)
y02 = f02(x)
ott.assert_almost_equal(y02, [1, 3])

# Direct coverage of ConstantEvaluation
defaultEvaluation = ot.ConstantEvaluation()
assert defaultEvaluation.getInputDimension() == 1
assert defaultEvaluation.getOutputDimension() == 1
ott.assert_almost_equal(defaultEvaluation([0.0]), [0.0])
_ = str(defaultEvaluation)
_ = repr(defaultEvaluation)
evaluation = ot.ConstantEvaluation(inputDimension, constant)
ott.assert_almost_equal(evaluation.getConstant(), constant)
assert evaluation.getInputDimension() == inputDimension
assert evaluation.getOutputDimension() == len(constant)
_ = str(evaluation)
_ = repr(evaluation)
assert evaluation.isActualImplementation()
assert evaluation == evaluation
assert not (evaluation == "dummy")
assert evaluation != "dummy"
with ott.assert_raises(Exception):
    evaluation == ot.LinearEvaluation()
assert not evaluation.isLinear()
assert not evaluation.isLinearlyDependent(0)
ott.assert_almost_equal(evaluation(x), constant)
sampleIn = ot.Sample([x, x])
sampleOut = evaluation(sampleIn)
assert sampleOut.getSize() == 2
ott.assert_almost_equal(sampleOut, [constant, constant])
callsBefore = evaluation.getCallsNumber()
evaluation(x)
assert evaluation.getCallsNumber() == callsBefore + 1
assert evaluation.getParameterDimension() == 0
assert evaluation.getParameter().getDimension() == 0
evaluation.setParameter([0.5])
assert evaluation.getParameterDimension() == 1
_ = evaluation.parameterGradient(x)
evaluation.setParameter(ot.Point())
assert evaluation.getParameterDescription().getSize() == 0
evaluation.setParameterDescription(["p0"])
assert evaluation.getParameterDescription()[0] == "p0"
marginal0 = evaluation.getMarginal(0)
assert marginal0.getOutputDimension() == 1
ott.assert_almost_equal(marginal0(x), [constant[0]])
marginal02 = evaluation.getMarginal([0, 2])
ott.assert_almost_equal(marginal02(x), [constant[0], constant[2]])
fullMarginal = evaluation.getMarginal([0, 1, 2])
assert fullMarginal.getOutputDimension() == evaluation.getOutputDimension()
ott.assert_almost_equal(fullMarginal(x), evaluation(x))
with ott.assert_raises(Exception):
    evaluation.getMarginal(5)
with ott.assert_raises(Exception):
    evaluation.getMarginal([0, 0])
with ott.assert_raises(Exception):
    ot.ConstantEvaluation(inputDimension, [])
with ott.assert_raises(Exception):
    evaluation([1.0])
with ott.assert_raises(Exception):
    evaluation(ot.Sample(2, 1))
with ott.assert_raises(Exception):
    evaluation.isLinearlyDependent(inputDimension)

# Extra Function-level coverage
ott.assert_almost_equal(f(ot.Sample([x, x])), [constant, constant])
ott.assert_almost_equal(f.getEvaluation()(x), constant)
assert not f.isLinear()
assert f.getCallsNumber() >= 1
assert not (f == "dummy")
assert f != "dummy"
with ott.assert_raises(Exception):
    f([1.0])
with ott.assert_raises(Exception):
    f.getMarginal(5)
