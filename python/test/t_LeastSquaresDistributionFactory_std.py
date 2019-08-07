#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(3)
distribution = ot.Pareto(3.3, 7.5, 0.0)
print('distribution=', distribution)
sample = distribution.getSample(1000)
factory = ot.LeastSquaresDistributionFactory(ot.Pareto())
factory.setKnownParameter([0.0], [2])
inf_distribution = factory.build(sample)
print('estimated distribution=', inf_distribution)
