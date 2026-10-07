#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

eps = 1e-2
# Instance creation
myFunc = ot.SymbolicFunction(
    ["x1", "x2"], ["x1*sin(x2)", "cos(x1+x2)", "(x2+1)*exp(x1-2*x2)"]
)
epsilon = ot.Point(myFunc.getInputDimension(), eps)
inPoint = ot.Point(epsilon.getDimension(), 1.0)
myHessian = ot.CenteredFiniteDifferenceHessian(epsilon, myFunc.getEvaluation())

print("myHessian=", repr(myHessian))
print("myFunc.hessian(", repr(inPoint), ")=", repr(myFunc.hessian(inPoint)))
print("myHessian.hessian(", repr(inPoint), ")=", repr(myHessian.hessian(inPoint)))

# Substitute the hessian
myFunc.setHessian(ot.CenteredFiniteDifferenceHessian(myHessian))
print(
    "myFunc.hessian(",
    repr(inPoint),
    ")=",
    repr(myFunc.hessian(inPoint)),
    " (after substitution)",
)

# Base class FiniteDifferenceHessian coverage
evaluation = myFunc.getEvaluation()
hessian = ot.FiniteDifferenceHessian(epsilon, evaluation)
print("base epsilon=", hessian.getEpsilon())
hessian = ot.FiniteDifferenceHessian(eps, evaluation)
print("base epsilon=", hessian.getEpsilon())
hessian = ot.FiniteDifferenceHessian(ot.ConstantStep(epsilon), evaluation)
print("base evaluation=", hessian.getEvaluation().getClassName())
print(
    "base inputDim=",
    hessian.getInputDimension(),
    "outputDim=",
    hessian.getOutputDimension(),
)
print("base repr=", repr(hessian))
with ott.assert_raises(ValueError):
    ot.FiniteDifferenceHessian(ot.Point(3, eps), evaluation)
with ott.assert_raises(TypeError):
    ot.FiniteDifferenceHessian(ot.Point([eps, 0.0]), evaluation)
with ott.assert_raises(TypeError):
    ot.FiniteDifferenceHessian(0.0, evaluation)
with ott.assert_raises(ValueError):
    ot.FiniteDifferenceHessian(ot.ConstantStep(ot.Point(3, eps)), evaluation)
with ott.assert_raises(TypeError):
    ot.FiniteDifferenceHessian(
        ot.ConstantStep(ot.Point([eps, 0.0])), evaluation
    )
with ott.assert_raises(RuntimeError):
    hessian.hessian(inPoint)
assert hessian == ot.FiniteDifferenceHessian(epsilon, evaluation)
assert hessian != ot.FiniteDifferenceHessian(ot.Point(2, 1e-5), evaluation)
hessian.setFiniteDifferenceStep(ot.ConstantStep(ot.Point(2, 1e-5)))
print("base step=", hessian.getFiniteDifferenceStep().getEpsilon())
print("base default epsilon=", ot.FiniteDifferenceHessian().getEpsilon())
study = ot.Study()
study.setStorageManager(ot.XMLStorageManager("finiteDifferenceHessian.xml"))
study.add("hessian", hessian)
study.save()
loaded = ot.Study()
loaded.setStorageManager(ot.XMLStorageManager("finiteDifferenceHessian.xml"))
loaded.load()
assert loaded.hasObject("hessian")
