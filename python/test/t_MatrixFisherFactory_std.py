#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

factory = otexp.MatrixFisherFactory()

# Default build
default = factory.build()
assert default.getDimension() == 9
assert ot.Distribution(default).getImplementation().getClassName() == \
    "MatrixFisher"

# Build from a sample of flattened rotation matrices
F = ot.SquareMatrix([[1.0, 0.2, -0.1], [0.0, 0.8, 0.3],
                     [0.1, -0.2, 0.5]])
reference = otexp.MatrixFisher(F)
ot.RandomGenerator.SetSeed(0)
sample = reference.getSample(3000)
estimated = factory.build(sample)
assert estimated.getDimension() == 9
experimental_estimated = otexp.MatrixFisher()
experimental_estimated.setParameter(estimated.getParameter())
ott.assert_almost_equal(experimental_estimated.getF(), F, 0.0, 0.5)
assert ot.Distribution(estimated).getImplementation().getClassName() == \
    "MatrixFisher"

# Build from parameters
dist = otexp.MatrixFisher(F)
param = dist.getParameter()
estimated2 = factory.build(param)
ott.assert_almost_equal(estimated2.getImplementation().getParameter(),
                        param, 1e-9, 0.0)

# buildAsMatrixFisher variants
native = factory.buildAsMatrixFisher(sample)
assert ot.Distribution(native).getImplementation().getClassName() == \
    "MatrixFisher"
native_param = factory.buildAsMatrixFisher(param)
ott.assert_almost_equal(native_param.getParameter(), param, 1e-9, 0.0)
native_default = factory.buildAsMatrixFisher()
assert ot.Distribution(native_default).getImplementation().getClassName() == \
    "MatrixFisher"

# Invalid samples
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0, 0.0, 0.0]] * 5))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0] * 9]))
with ott.assert_raises(TypeError):
    factory.build([1.0] * 8)
