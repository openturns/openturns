#! /usr/bin/env python

import openturns as ot

ot.TESTPREAMBLE()

dimension = 2
epsilon = ot.Point(dimension, 1e-4)
x = ot.Point(dimension, 2.0)

step = ot.ConstantStep(epsilon)
print('step type=', step.getClassName(), 'step value=', step(x))

eta = ot.Point(dimension, 1.0)
step = ot.BlendedStep(epsilon, eta)
print('step type=', step.getClassName(), 'step value=', step(x))
