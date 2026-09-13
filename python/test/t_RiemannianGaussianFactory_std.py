#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

factory = otexp.RiemannianGaussianFactory()

# Default build
default = factory.build()
assert default.getDimension() == 3
assert ot.Distribution(default).getImplementation().getClassName() == \
    "RiemannianGaussian"

# Build from a sample of flattened SPD matrices (n = 2, d = 3)
mean = ot.SymmetricMatrix([[2.0, 0.0], [0.0, 1.0]])
sigma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
reference = otexp.RiemannianGaussian(mean, sigma)
ot.RandomGenerator.SetSeed(0)
sample = reference.getSample(3000)
estimated = factory.build(sample)
assert estimated.getDimension() == 3
experimental_estimated = otexp.RiemannianGaussian()
experimental_estimated.setParameter(estimated.getParameter())
mean_est = experimental_estimated.getMeanMatrix()
ott.assert_almost_equal(mean_est[0, 0], 2.0, 0.5, 0.0)
ott.assert_almost_equal(mean_est[1, 1], 1.0, 0.5, 0.0)
assert ot.Distribution(estimated).getImplementation().getClassName() == \
    "RiemannianGaussian"

# Build from parameters
dist = otexp.RiemannianGaussian(mean, sigma)
param = dist.getParameter()
estimated2 = factory.build(param)
ott.assert_almost_equal(estimated2.getImplementation().getParameter(),
                        param, 1e-9, 0.0)

# buildAsRiemannianGaussian variants
native = factory.buildAsRiemannianGaussian(sample)
assert ot.Distribution(native).getImplementation().getClassName() == \
    "RiemannianGaussian"
native_param = factory.buildAsRiemannianGaussian(param)
ott.assert_almost_equal(native_param.getParameter(), param, 1e-9, 0.0)
native_default = factory.buildAsRiemannianGaussian()
assert ot.Distribution(native_default).getImplementation().getClassName() == \
    "RiemannianGaussian"

# Invalid samples: dimension must be n (n + 1) / 2
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0] * 2] * 5))
with ott.assert_raises(TypeError):
    factory.build([1.0] * 5)
