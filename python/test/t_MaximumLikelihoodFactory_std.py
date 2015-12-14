#! /usr/bin/env python

from __future__ import print_function
import openturns as ot

ot.PlatformInfo.SetNumericalPrecision(3)
inner_factory = ot.NormalFactory()
sample = inner_factory.build().getSample(1000)
factory = ot.MaximumLikelihoodFactory(inner_factory)
distribution = factory.build(sample)
print(distribution)
