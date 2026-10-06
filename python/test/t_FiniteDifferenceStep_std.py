#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

dimension = 2
epsilon = ot.Point(dimension, 1e-4)
x = ot.Point(dimension, 2.0)

step = ot.ConstantStep(epsilon)
print("step type=", step.getClassName(), "step value=", step(x))
print("repr=", repr(step))
assert step == ot.ConstantStep(epsilon)
assert step != ot.ConstantStep(ot.Point(dimension, 1e-5))
print("default epsilon=", ot.ConstantStep().getEpsilon())

# Dimension mismatch raises
with ott.assert_raises(TypeError):
    step(ot.Point(1, 2.0))

eta = ot.Point(dimension, 1.0)
step = ot.BlendedStep(epsilon, eta)
print("step type=", step.getClassName(), "step value=", step(x))
