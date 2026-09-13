#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()

factory = otexp.BinghamFactory()

# Default build
default = factory.build()
assert default.getDimension() == 3
assert ot.Distribution(default).getImplementation().getClassName() == \
    "Bingham"

# Build from a uniform sample on the sphere: the estimated concentrations
# must be close to zero (sample noise only)
gamma = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0],
                         [0.0, 0.0, 1.0]])
uniform = otexp.Bingham([0.0, 0.0, 0.0], gamma)
ot.RandomGenerator.SetSeed(0)
sample = uniform.getSample(2000)

estimated = factory.build(sample)
assert estimated.getDimension() == 3
zi = estimated.getImplementation().getZeta()
for z in zi:
    assert abs(z) < 0.3, "zeta=%s" % zi
assert ot.Distribution(estimated).getImplementation().getClassName() == \
    "Bingham"

# Build from a concentrated sample: the estimated distribution must be able
# to describe the anisotropic scatter of the sample
reference = otexp.Bingham([1.5, 0.5, 0.0], gamma)
ot.RandomGenerator.SetSeed(0)
sample = reference.getSample(3000)
estimated = factory.build(sample)
est_cov = ot.Distribution(estimated).getImplementation().getCovariance()
sample_cov = sample.computeCovariance()
# The trace of the covariance is one both for the sample and the estimate
estimate_trace = est_cov[0, 0] + est_cov[1, 1] + est_cov[2, 2]
sample_trace = sample_cov[0, 0] + sample_cov[1, 1] + sample_cov[2, 2]
ott.assert_almost_equal(estimate_trace, 1.0, 1e-5, 0.0)
ott.assert_almost_equal(sample_trace, 1.0, 1e-3, 0.0)

# Build from parameters
dist = otexp.Bingham([1.0, 0.5, 0.0], gamma)
param = dist.getParameter()
estimated2 = factory.build(param)
ott.assert_almost_equal(estimated2.getImplementation().getParameter(),
                        param, 1e-9, 0.0)

# buildAsBingham variants
native = factory.buildAsBingham(sample)
assert ot.Distribution(native).getImplementation().getClassName() == "Bingham"
native_param = factory.buildAsBingham(param)
ott.assert_almost_equal(native_param.getParameter(), param, 1e-9, 0.0)
native_default = factory.buildAsBingham()
assert ot.Distribution(native_default).getImplementation().getClassName() == \
    "Bingham"

# Invalid samples
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0]] * 5))
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[1.0, 0.0]] * 2))
with ott.assert_raises(TypeError):
    factory.build([1.0, 2.0])
