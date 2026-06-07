#! /usr/bin/env python

import openturns as ot
import openturns.experimental as otexp
import openturns.testing as ott

ot.TESTPREAMBLE()
ot.RandomGenerator.SetSeed(0)

# Default constructor
factory = otexp.KentFactory()
print("Factory = ", factory)

# Default build
default = factory.buildAsKent()
print("Default = ", default)
ott.assert_almost_equal(default.getKappa(), 1.0)
ott.assert_almost_equal(default.getBeta(), 0.25)

# Build from parameters
from_params = factory.buildAsKent(
    [10.0, 0.25, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
)
print("From params = ", from_params)
ott.assert_almost_equal(from_params.getKappa(), 10.0)
ott.assert_almost_equal(from_params.getBeta(), 0.25)

# Build from sample with identity orientation
gamma_matrix = ot.SquareMatrix([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])
true_dist = otexp.Kent(25.0, 0.5, gamma_matrix)
sample = true_dist.getSample(200000)
estimated_dist = factory.buildAsKent(sample)
print("Estimated = ", estimated_dist)
ott.assert_almost_equal(estimated_dist.getKappa(), 25.0, 2e-1, 0.0)
ott.assert_almost_equal(estimated_dist.getBeta(), 0.5, 2e-1, 0.0)

# Check estimated orientation is close to identity
gamma_est = estimated_dist.getGamma()
ott.assert_almost_equal(gamma_est[0, 0], 1.0, 1e-2, 0.0)
ott.assert_almost_equal(gamma_est[1, 1], 1.0, 1e-2, 0.0)
ott.assert_almost_equal(gamma_est[2, 2], 1.0, 1e-2, 0.0)

# Build from sample with rotated orientation
theta = 0.3
cos_t = __import__("math").cos(theta)
sin_t = __import__("math").sin(theta)
rotated_gamma = ot.SquareMatrix(
    [[cos_t, -sin_t, 0.0], [sin_t, cos_t, 0.0], [0.0, 0.0, 1.0]]
)
rotated_dist = otexp.Kent(20.0, 0.3, rotated_gamma)
rotated_sample = rotated_dist.getSample(200000)
rotated_est = factory.buildAsKent(rotated_sample)
print("Rotated estimated = ", rotated_est)
ott.assert_almost_equal(rotated_est.getKappa(), 20.0, 2e-1, 0.0)
ott.assert_almost_equal(rotated_est.getBeta(), 0.3, 2e-1, 0.0)

# Build from sample with beta=0 (von Mises-Fisher)
vmf_dist = otexp.Kent(15.0, 0.0, gamma_matrix)
vmf_sample = vmf_dist.getSample(200000)
vmf_est = factory.buildAsKent(vmf_sample)
print("VMF estimated = ", vmf_est)
ott.assert_almost_equal(vmf_est.getKappa(), 15.0, 2e-1, 0.0)
ott.assert_almost_equal(vmf_est.getBeta(), 0.0, 0.0, 2e-2)

# Generic build interface
generic_dist = factory.build(sample)
print("Generic build type = ", generic_dist.getImplementation().getClassName())
assert generic_dist.getImplementation().getClassName() == "Kent"

# Build from too few points should fail
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[0.0, 0.0, 0.0], [1.0, 0.0, 0.0]]))

# Build from wrong dimension should fail
with ott.assert_raises(TypeError):
    factory.build(ot.Sample([[0.0], [1.0], [2.0]]))

# Build from invalid parameters should fail
with ott.assert_raises(TypeError):
    factory.buildAsKent([-1.0, 0.25, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0])
