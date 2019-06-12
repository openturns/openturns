#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(3)

size = 10000

distribution = ot.Gumbel(1.5, -0.5)
print('distribution=', distribution)
sample = distribution.getSample(size)
factory = ot.MethodOfMomentsFactory(ot.Gumbel())
inf_distribution = factory.build(sample)
print('estimated distribution=', inf_distribution)

# set (a,b) out of (r, t, a, b)
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
print('distribution=', distribution)
sample = distribution.getSample(size)
factory = ot.MethodOfMomentsFactory(ot.Beta())
factory.setKnownParameter([-1.0, 1.0], [2, 3])
inf_distribution = factory.build(sample)
print('estimated distribution=', inf_distribution)
