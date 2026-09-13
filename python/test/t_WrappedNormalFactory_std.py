#! /usr/bin/env python

import math

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

factory = otexp.WrappedNormalFactory()

# Default build
default = factory.build()
assert default.getDimension() == 2
assert ot.Distribution(default).getImplementation().getClassName() == \
    "WrappedNormal"

# The default period comes from the ResourceMap key
ott.assert_almost_equal(
    factory.buildAsWrappedNormal().getPeriod(),
    ot.ResourceMap.GetAsScalar("WrappedNormalFactory-DefaultPeriod"),
    1e-12,
    0.0,
)

# Build from a sample on the circle
mu = 0.4
sigma = ot.CovarianceMatrix([[0.25]])
reference = otexp.WrappedNormal([mu], sigma)
ot.RandomGenerator.SetSeed(0)
sample = reference.getSample(5000)
estimated = factory.build(sample)
assert estimated.getDimension() == 1
experimental_estimated = factory.buildAsWrappedNormal(sample)
ott.assert_almost_equal(experimental_estimated.getMu()[0], mu, 0.1, 0.0)
ott.assert_almost_equal(experimental_estimated.getPeriod(),
                        2.0 * math.pi, 1e-9, 0.0)
assert ot.Distribution(estimated).getImplementation().getClassName() == \
    "WrappedNormal"

# Build from a sample on the torus (dimension 2)
reference2 = otexp.WrappedNormal([0.1, -0.2], ot.CovarianceMatrix(2))
ot.RandomGenerator.SetSeed(0)
sample2 = reference2.getSample(1000)
estimated2 = factory.build(sample2)
assert estimated2.getDimension() == 2

# Build from parameters
dist = otexp.WrappedNormal([mu], sigma)
param = dist.getParameter()
estimated3 = factory.build(param)
ott.assert_almost_equal(estimated3.getImplementation().getParameter(),
                        param, 1e-9, 0.0)

# buildAsWrappedNormal variants
native = factory.buildAsWrappedNormal(sample)
assert ot.Distribution(native).getImplementation().getClassName() == \
    "WrappedNormal"
native_param = factory.buildAsWrappedNormal(param)
ott.assert_almost_equal(native_param.getParameter(), param, 1e-9, 0.0)
native_default = factory.buildAsWrappedNormal()
assert ot.Distribution(native_default).getImplementation().getClassName() == \
    "WrappedNormal"

# Invalid samples
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0, 2.0]] * 2))
with ott.assert_raises(TypeError):
    factory.build([1.0, 2.0])
