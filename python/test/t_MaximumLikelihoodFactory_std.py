#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(3)
distribution = ot.Beta(2.3, 2.2, -1.0, 1.0)
print('distribution=', distribution)
sample = distribution.getSample(1000)
factory = ot.MaximumLikelihoodFactory(ot.Beta())
inf_distribution = factory.build(sample)
print('estimated distribution=', inf_distribution)

# set (a,b) out of (r, t, a, b)
factory.setKnownParameter([-1.0, 1.0], [2, 3])
inf_distribution = factory.build(sample)
print('estimated distribution with bounds=', inf_distribution)
