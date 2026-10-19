#! /usr/bin/env python

import openturns as ot
import openturns.testing as ott

ot.TESTPREAMBLE()

# Keyword arguments on distributions beyond Normal
uniform = ot.Uniform(a=-0.5, b=1.5)
assert uniform == ot.Uniform(-0.5, 1.5)

# Note: C++ parameter `lambda` is a Python keyword; SWIG exposes it as
# `_lambda` (see Exponential_setLambda), and the sidecar follows suit.
exponential = ot.Exponential(_lambda=2.5, gamma=-0.5)
assert exponential == ot.Exponential(2.5, -0.5)

gumbel = ot.Gumbel(beta=1.5, gamma=0.5)
assert gumbel == ot.Gumbel(1.5, 0.5)

beta = ot.Beta(alpha=2.0, beta=3.0, a=-1.0, b=1.0)
assert beta == ot.Beta(2.0, 3.0, -1.0, 1.0)

# Inherited Distribution methods accept keywords
point = ot.Point([0.5])
assert uniform.computePDF(point=point) == uniform.computePDF(point)
assert uniform.computeCDF(point=point) == uniform.computeCDF(point)
assert uniform.computeQuantile(prob=0.5) == uniform.computeQuantile(0.5)

sample = ot.Sample([point])
assert exponential.computePDF(sample=sample) == exponential.computePDF(sample)

# Unknown keywords are rejected
with ott.assert_raises(TypeError):
    ot.Uniform(unknown=0.5)
with ott.assert_raises(TypeError):
    ot.Exponential(unknown=0.5)
